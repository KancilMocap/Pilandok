#include "kijangclientmodule.h"

KijangClientModule::KijangClientModule(QObject *parent) : QObject(parent)
{

}

quint32 KijangClientModule::module() const
{
    return m_module;
}
