#!/usr/bin/python3.8
from distutils.core import setup, Extension

def main():
	setup(name='pyeggnogg', 
	version='1.0', 
	author="0xukn",
	author_email="0xUKN@protonmail.com",
	description="Python client for the libeggnogg instrumentation library",
	url='https://github.com/0xUKN/libeggnogg',
	ext_modules=[Extension('pyeggnogg', ['src/pyeggnogg.cpp', 'src/rpc/libeggnogg_rpc_clnt.cpp', '../lib/libinjector/src/Utils.cpp', '../lib/libinjector/src/TracedProcess.cpp', '../lib/libeggnogg_shmem/src/libeggnogg_shmem.cpp'], extra_link_args=["-lrt"])])

if __name__ == "__main__":
	main()
