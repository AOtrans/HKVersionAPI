#ifndef PYLOADER_H
#define PYLOADER_H
#include<Python.h>
#include<QDebug>
#include<tools/conversion.h>

class PYLoader
{
public:
    PYLoader();
    ~PYLoader();
    bool initPY(int argc, char *argv[]);
    PyObject* callPyMethod(PyObject* para, PyObject* para2);
private:
    PyObject* pModule =  NULL;
    PyObject* pDict = NULL;
    PyObject* pClass = NULL;
    PyObject* pInstance_hi_class = NULL;
};

#endif // PYLOADER_H
