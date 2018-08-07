#ifndef PYLOADER_H
#define PYLOADER_H
#include<Python.h>
#include<QDebug>

class PYLoader
{
public:
    PYLoader();
    ~PYLoader();
    bool initPY();
    PyObject* callPyMethod(PyObject* para);
private:
    PyObject* pModule =  NULL;
    PyObject* pDict = NULL;
    PyObject* pClass = NULL;
    PyObject* pInstance_hi_class = NULL;
};

#endif // PYLOADER_H
