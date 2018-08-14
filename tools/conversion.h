# ifndef __COVERSION_OPENCV_H__
# define __COVERSION_OPENCV_H__

#include <Python.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "numpy/ndarrayobject.h"

class PyEnsureGIL
{
public:
    PyEnsureGIL()
    {
        _state = PyGILState_Ensure();
    }
    ~PyEnsureGIL()
    {
        PyGILState_Release(_state);
    }
private:
    PyGILState_STATE _state;
};


static size_t REFCOUNT_OFFSET = (size_t)&(((PyObject*)0)->ob_refcnt) +
    (0x12345678 != *(const size_t*)"\x78\x56\x34\x12\0\0\0\0\0")*sizeof(int);

static inline PyObject* pyObjectFromRefcount(const int* refcount)
{
    return (PyObject*)((size_t)refcount - REFCOUNT_OFFSET);
}

static inline int* refcountFromPyObject(const PyObject* obj)
{
    return (int*)((size_t)obj + REFCOUNT_OFFSET);
}


class NumpyAllocator;


class NDArrayConverter
{
public:
    NDArrayConverter();
    cv::Mat toMat(const PyObject* o);
    PyObject* toNDArray(const cv::Mat& mat);
};

# endif
