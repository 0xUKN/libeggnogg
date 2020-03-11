#include <python3.8/Python.h>
#include "../lib/libeggnogg_shmem/include/libeggnogg_shmem.hpp"

namespace LibEggnogg
{
	static 	CLIENT *clnt;
	static GameState* gs;
	void exit_rpc_clnt_wrapper();

	static PyObject* init(PyObject* self, PyObject* args);
	static PyObject* setSpeed(PyObject* self, PyObject* args);
	static PyObject* getSpeed(PyObject* self);

	PyMODINIT_FUNC PyInit_pyeggnogg(void);
}
