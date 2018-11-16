#include "pyloader.h"
#include <QDebug>
#include <QThread>
#include "common.h"

PYLoader::PYLoader()
{

}

PYLoader::~PYLoader()
{

}

bool PYLoader::initPY(int argc, char *argv[])
{
    Py_Initialize(); // 初始化，这是必须的，用来初始化python所需的环境
    if (!Py_IsInitialized())
    {
        PyErr_Print();
        return false;
    }

#ifdef THREAD_ABLE
    //make sure thread surpport
    PyEval_InitThreads();
    int nInit = PyEval_ThreadsInitialized();

    if(!nInit)
    {
        PyErr_Print();
        return false;
    }
#endif

    // 导入模块
    PySys_SetArgv(argc, argv);
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString(QString("sys.path.append('%1')").arg(PY_ROOT_PATH).toStdString());
    //drop some warnings
    PyRun_SimpleString("os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'");

    //load target module
    pModule = PyImport_ImportModule("Photo_Dectect");

    if (!pModule) {
        qDebug("Cant find py file!\n");
        PyErr_Print();
        return false;
    }
    // 模块的字典列表
    pDict = PyModule_GetDict(pModule);
    if (!pDict) {
        qDebug("Cant load module dictionary.\n");
        PyErr_Print();
        return false;
    }

    //find target class
    pClass = PyDict_GetItemString(pDict, "Photo_Dectect");
    if (!pClass) {
        qDebug("Cant find class Photo_Dectect.\n");
        PyErr_Print();
        return false;
    }
    //构造hi_class的实例
    pInstance_hi_class = PyInstance_New(pClass , NULL, NULL);
    if (!pInstance_hi_class) {
        qDebug("Cant create instance.\n");
        PyErr_Print();
        return false;
    }

//    // 函数调用
//    qDebug()<<"calling python program..."<<endl;
//    PyObject* pFunHi = PyDict_GetItemString(pDict, "test");
//    PyObject* phi_class = PyObject_CallFunction(pFunHi, NULL, NULL);
//    if (!phi_class ) {
//        printf("Cant find phi_class class.\n");
//        return -1;
//    }

#ifdef THREAD_ABLE
    //release thread that make sure other sub threads able to get GIL
    PyEval_ReleaseThread(PyThreadState_Get());
#endif

    qDebug() << "py init done";
    return true;
}

void PYLoader::deinitPY()
{
    PyGILState_Ensure();
    qDebug() << "destory py";
    if(pClass)
        Py_DECREF(pClass);
    if(pInstance_hi_class)
        Py_DECREF(pInstance_hi_class );
    if(pModule)
        Py_DECREF(pModule);
    if(pDict)
        Py_DECREF(pDict);


    Py_Finalize(); // 与初始化对应
}

PyObject* PYLoader::callPyMethod(PyObject* para, PyObject* para2)
{
    if(pInstance_hi_class == NULL)
    {
        return NULL;
    }
    else
    {
        //调用hi_class类实例pInstance_hi_class里面的方法
        qDebug() << "call pyfunc" << "currentThread:" << QThread::currentThread();
        PyObject *ret = PyObject_CallMethod(pInstance_hi_class , "predict", "OO", para, para2);
        if(!ret)
        {
            qDebug() << "call func failed";
            PyErr_Print();
        }
        else
        {
            qDebug() << "call func successed";
        }

        return ret;
    }
}
