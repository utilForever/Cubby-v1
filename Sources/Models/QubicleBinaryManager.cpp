/*************************************************************************
> File Name: QubicleBinaryManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Qubicle binary manager.
> Created Time: 2016/07/02
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "QubicleBinaryManager.h"

QubicleBinaryManager::QubicleBinaryManager(Renderer* pRenderer)
{
	m_pRenderer = pRenderer;
}

QubicleBinaryManager::~QubicleBinaryManager()
{
	ClearQubicleBinaryList();
}

void QubicleBinaryManager::ClearQubicleBinaryList()
{
	for (unsigned int i = 0; i < m_vpQubicleBinaryList.size(); ++i)
	{
		delete m_vpQubicleBinaryList[i];
		m_vpQubicleBinaryList[i] = nullptr;
	}

	m_vpQubicleBinaryList.clear();
}

QubicleBinary* QubicleBinaryManager::GetQubicleBinaryFile(const char* fileName, bool refreshModel)
{
	for (unsigned int i = 0; i < m_vpQubicleBinaryList.size(); ++i)
	{
		if (strcmp(m_vpQubicleBinaryList[i]->GetFileName().c_str(), fileName) == 0)
		{
			if (refreshModel)
			{
				m_vpQubicleBinaryList[i]->Reset();
				m_vpQubicleBinaryList[i]->Import(fileName, true);
			}

			return m_vpQubicleBinaryList[i];
		}
	}

	return AddQubicleBinaryFile(fileName);
}

QubicleBinary* QubicleBinaryManager::AddQubicleBinaryFile(const char* fileName)
{
	QubicleBinary* pNewQubicleBinary = new QubicleBinary(m_pRenderer);
	pNewQubicleBinary->Import(fileName, true);

	m_vpQubicleBinaryList.push_back(pNewQubicleBinary);

	return pNewQubicleBinary;
}