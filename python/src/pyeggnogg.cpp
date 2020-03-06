#include <python3.8/Python.h>
#include <iostream>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include "../../lib/libinjector/include/TracedProcess.hpp"
#include "../../include/libeggnogg_rpc.hpp"
//#define DEBUG

namespace LibEggnogg
{
		static 	CLIENT *clnt;

		static PyObject* init(PyObject* self, PyObject* args) {
		char library_path[PATH_MAX];
		char executable_path[PATH_MAX];
		char* library_name;
		char* executable_name;
		if(!PyArg_ParseTuple(args, "ss", &library_name, &executable_name))
		{
			return nullptr;
		}

		if(realpath(library_name, library_path) == NULL || access(library_path, F_OK) == -1)
		{
			PyErr_SetString(PyExc_RuntimeError, "Library not found");
			return nullptr;
		}
		library_name = strrchr(library_path, '/') + 1;
		if(realpath(executable_name, executable_path) == NULL || access(executable_path, F_OK) == -1)
		{
			PyErr_SetString(PyExc_RuntimeError, "Executable not found");
			return nullptr;
		}
		executable_name = strrchr(executable_path, '/') + 1;

		LibraryInjector::TracedProcess* proc = LibraryInjector::Attach(executable_name);
		if(proc == NULL) 
		{ 
			std::cerr << "[+] " << executable_name << " is not running ..." << std::endl;
			std::cout << "[+] Starting " << executable_name << " ..." << std::endl;
			pid_t pid = fork();
			char * argv[] = {executable_name, NULL};
			if (pid == 0) {
				setenv("LD_PRELOAD", library_path, 1);
				chdir(executable_path);
				setsid();
				signal(SIGHUP, SIG_IGN);
				#ifndef DEBUG
				//close(0);
				//close(1);
				//close(2);
				#endif
				execve(executable_path, argv, environ);	
			}
			std::cout << "[+] PID : " << pid << std::endl;
		}
		else
		{
			std::cout << "[+] PID : " << proc->getPID() << std::endl;
			if(proc->LoadLibrary(library_path) == -1)
			{
					PyErr_SetString(PyExc_RuntimeError, ("[-] Failed to inject " + std::string(library_name) + " in " + std::string(executable_name)).c_str());
					delete proc;
					return Py_False;
			}
		}
		std::cout << "[+] Successfully injected " << library_name << " in " << executable_name << std::endl;
		delete proc;

		sleep(1);
		clnt = clnt_create("localhost", LibEggnogg_RPC, Stable, "udp");
		if (clnt == NULL) {
			clnt_pcreateerror("localhost");
			PyErr_SetString(PyExc_RuntimeError, "[-] Failed to initialize RPC client");
			return nullptr;
		}
		return Py_True;
	}

	static char init_docs[] = "init(): Load eggnogg library in process memory\n";

	static PyObject* setSpeed(PyObject* self, PyObject* args) {
		void  *result_1;
		u_long  set_speed_3_arg;
		if(!PyArg_ParseTuple(args, "l", &set_speed_3_arg))
		{
			return nullptr;
		}
		result_1 = set_speed_3(&set_speed_3_arg, clnt);
		if (result_1 == (void *) NULL) {
			PyErr_SetString(PyExc_RuntimeError, "[-] setSpeed : RPC call failed");
			return nullptr;
		}
		return Py_BuildValue("");
	}

	static char setSpeed_docs[] = "setSpeed(speed): Set game speed\n";

	static PyObject* getSpeed(PyObject* self) {
		u_long  *result_2;
		char *get_speed_3_arg;
		result_2 = get_speed_3((void*)&get_speed_3_arg, clnt);
		if (result_2 == (u_long *) NULL) {
			PyErr_SetString(PyExc_RuntimeError, "[-] getSpeed : RPC call failed");
			return nullptr;
		}
		return Py_BuildValue("l", *result_2);
	}

	static char getSpeed_docs[] = "getSpeed(): Get game speed\n";

	static PyMethodDef pyeggnogg_funcs[] = { 
		{"init", (PyCFunction)init, METH_VARARGS, init_docs}, 
		{"setSpeed", (PyCFunction)setSpeed, METH_VARARGS, setSpeed_docs}, 
		{"getSpeed", (PyCFunction)getSpeed, METH_NOARGS, getSpeed_docs},
		{NULL}
	};

	static struct PyModuleDef moduledef = {
		PyModuleDef_HEAD_INIT,
		"pyeggnogg",     /* m_name */
		"Additional functions for eggnoggplus",  /* m_doc */
		-1,                  /* m_size */
		pyeggnogg_funcs,    /* m_methods */
		NULL,                /* m_reload */
		NULL,                /* m_traverse */
		NULL,                /* m_clear */
		NULL,                /* m_free */
	};

	PyMODINIT_FUNC PyInit_pyeggnogg(void)
	{
	    return PyModule_Create(&moduledef);
	}
}
