#include "GlobalSignal.h"

GlobalSignal * GlobalSignal::m_instance = nullptr;

GlobalSignal::GlobalSignal()
{

}

GlobalSignal* GlobalSignal::getInstance()
{
	if (!m_instance)
	{
		m_instance = new GlobalSignal();
	}
	return m_instance;
}