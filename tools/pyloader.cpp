#include "pyloader.h"

PYLoader::PYLoader()
{

}

PYLoader::~PYLoader()
{
    Py_DECREF(pClass);
    Py_DECREF(pInstance_hi_class );
    Py_DECREF(pModule);
    Py_DECREF(pDict);

    Py_Finalize(); // 与初始化对应
}

bool PYLoader::initPY()
{
    Py_Initialize(); // 初始化，这是必须的，用来初始化python所需的环境
    if (!Py_IsInitialized())
        return false;
    // 导入模块
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('/home/zg/HKVision/tools')");
    pModule = PyImport_ImportModule("myclass");
    if (!pModule) {
        printf("Cant open python file!\n");
        return false;
    }
    // 模块的字典列表
    pDict = PyModule_GetDict(pModule);
    if (!pDict) {
        printf("Cant find dictionary.\n");
        return false;
    }

    pClass = PyDict_GetItemString(pDict, "hi_class");
    if (!pClass) {
        printf("Cant find hi_class.\n");
        return false;
    }
    //构造hi_class的实例
    pInstance_hi_class = PyInstance_New(pClass , NULL, NULL);
    if (!pInstance_hi_class) {
        printf("Cant create instance.\n");
        return false;
    }

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
        PyObject *ret = PyObject_CallMethod(pInstance_hi_class , "sayHi", "O", para);
        return ret;
    }
}
