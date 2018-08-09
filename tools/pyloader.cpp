#include "pyloader.h"
#include <QDebug>
PYLoader::PYLoader()
{

}

PYLoader::~PYLoader()
{
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

bool PYLoader::initPY(int argc, char *argv[])
{
    Py_Initialize(); // 初始化，这是必须的，用来初始化python所需的环境
    if (!Py_IsInitialized())
        return false;
    // 导入模块
    PySys_SetArgv(argc, argv);
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append('/home/zg/yaochang')");

    PyRun_SimpleString("os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'");
    pModule = PyImport_ImportModule("Photo_Dectect");

    //pModule = PyImport_ImportModule("myclass");
    if (!pModule) {
        qDebug("Cant open python file!\n");
        return false;
    }
    // 模块的字典列表
    pDict = PyModule_GetDict(pModule);
    if (!pDict) {
        qDebug("Cant find dictionary.\n");
        return false;
    }

    pClass = PyDict_GetItemString(pDict, "Photo_Dectect");
    if (!pClass) {
        qDebug("Cant find class Photo_Dectect.\n");
        return false;
    }
    //构造hi_class的实例
    pInstance_hi_class = PyInstance_New(pClass , NULL, NULL);
    if (!pInstance_hi_class) {
        qDebug("Cant create instance.\n");
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

    return true;
}

PyObject* PYLoader::callPyMethod(PyObject* para)
{
    if(pInstance_hi_class == NULL)
        return NULL;
    else
    {
        //调用hi_class类实例pInstance_hi_class里面的方法
        qDebug() << "call pyfunc";
        PyObject *ret = PyObject_CallMethod(pInstance_hi_class , "predict", "O", para);
        if(!ret)
        {
            qDebug() << "call func failed";
        }
        else
        {
            qDebug() << "call func successed";
        }
        return ret;
    }
}
