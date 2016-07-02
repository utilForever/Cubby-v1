/*************************************************************************
> File Name: QubicleBinaryManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Qubicle binary manager.
> Created Time: 2016/07/02
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_QUBICLE_BINARY_MANAGER_H
#define CUBBY_QUBICLE_BINARY_MANAGER_H

#include "QubicleBinary.h"

using QubicleBinaryList = std::vector<QubicleBinary*>;

class QubicleBinaryManager
{
public:
	// Constructor, Destructor
	QubicleBinaryManager(Renderer* pRenderer);
	~QubicleBinaryManager();

	void ClearQubicleBinaryList();

	// Getter
	QubicleBinary* GetQubicleBinaryFile(const char* fileName, bool refreshModel);

	// Operations
	QubicleBinary* AddQubicleBinaryFile(const char* fileName);

private:
	Renderer* m_pRenderer;
	QubicleBinaryList m_vpQubicleBinaryList;
};

#endif