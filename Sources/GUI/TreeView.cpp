/*************************************************************************
> File Name: TreeView.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A simple GUI component that simple displays a text label.
> Created Time: 2016/08/27
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DirectDrawRectangle.h"
#include "TreeView.h"

// Constructor, Destructor
TreeView::TreeView(Renderer* pRenderer, unsigned int GUIFont, std::string rootName) :
	Container(pRenderer), m_scrollBar(pRenderer)
{
	m_pRenderer = pRenderer;
	m_GUIFont = GUIFont;

	m_root = nullptr;

	m_pBackgroundDefault = new DirectDrawRectangle(pRenderer);
	m_pBackgroundDisabled = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pBackgroundDefault);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pBackgroundDisabled);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	Container::Add(m_pBackgroundDefault);
	Container::Add(m_pBackgroundDisabled);

	m_scrollBar.SetScrollDirection(ScrollBarDirection::Vertical);
	m_scrollBar.SetScrollPosition(0.75f);

	m_scrollBar.SetValueChangedCallback(_ScrollbarChanged);
	m_scrollBar.SetValueChangedCallBackData(this);

	Container::Add(&m_scrollBar);

	m_numOpenNodes = 0;

	AddRootNode(rootName);

	m_xIndent = 8;
	m_yOffset = -20;
}

TreeView::~TreeView()
{
	ClearTreeView();
}

void TreeView::ClearTreeView()
{
	ClearNode(m_root);

	m_root = nullptr;
}

void TreeView::ClearNode(TreeNode* pNode)
{
	for (int i = 0; i < pNode->m_vpChildren.size(); ++i)
	{
		ClearNode(pNode->m_vpChildren[i]);
	}
	pNode->m_vpChildren.clear();

	Remove(pNode->m_pName);
	delete pNode->m_pName;
	pNode->m_pName = nullptr;

	Remove(pNode->m_pExpandButton);
	delete pNode->m_pExpandButton;
	pNode->m_pExpandButton = nullptr;

	delete pNode;
}

void TreeView::SetRootName(std::string rootName) const
{
	m_root->m_pName->SetText(rootName);
}

TreeNode* TreeView::FindNode(std::string name, TreeNode* pNode) const
{
	if (pNode != nullptr)
	{
		if (pNode->m_pName->GetText() == name)
		{
			return pNode;
		}

		for (int i = 0; i < pNode->m_vpChildren.size(); ++i)
		{
			TreeNode* foundNode = FindNode(name, pNode->m_vpChildren[i]);

			if (foundNode != nullptr)
			{
				return foundNode;
			}
		}
	}

	return nullptr;
}

void TreeView::AddRootNode(std::string name)
{
	if (m_root == nullptr)
	{
		m_root = new TreeNode();
		m_root->m_pTreeView = this;
		m_root->m_pName = new Label(m_pRenderer, m_GUIFont, name, Color(0.0f, 0.0f, 0.0f, 1.0f));
		m_root->m_pExpandButton = new Button(m_pRenderer, m_GUIFont, "+");
		m_root->m_pExpandButton->SetCallBackFunction(_ExpandPressed);
		m_root->m_pExpandButton->SetCallBackData(m_root);
		m_root->m_pExpandButton->SetLabelOffset(0, 0);
		m_root->m_expanded = false;
		m_root->m_pParentNode = nullptr;

		Add(m_root->m_pName);
		Add(m_root->m_pExpandButton);

		m_root->m_pName->SetVisible(true);
		m_root->m_pExpandButton->SetVisible(true);
	}

	UpdateScrollbar();
}

void TreeView::AddChildNode(std::string parentName, std::string name)
{
	TreeNode* pNode = FindNode(parentName, m_root);

	if (pNode != nullptr)
	{
		TreeNode* pNewNode = new TreeNode();
		pNewNode->m_pTreeView = this;
		pNewNode->m_pName = new Label(m_pRenderer, m_GUIFont, name, Color(0.0f, 0.0f, 0.0f, 1.0f));
		pNewNode->m_pExpandButton = new Button(m_pRenderer, m_GUIFont, "+");
		pNewNode->m_pExpandButton->SetCallBackFunction(_ExpandPressed);
		pNewNode->m_pExpandButton->SetCallBackData(pNewNode);
		pNewNode->m_pExpandButton->SetLabelOffset(0, 0);
		pNewNode->m_expanded = false;
		pNewNode->m_pParentNode = pNode;

		Add(pNewNode->m_pName);
		Add(pNewNode->m_pExpandButton);

		pNewNode->m_pName->SetVisible(false);
		pNewNode->m_pExpandButton->SetVisible(false);

		pNode->m_vpChildren.push_back(pNewNode);
	}

	// Reset all component dimensions starting from the root node
	int yCul = -20 + m_yOffset;
	m_numOpenNodes = 0;
	SetNodeDimensions(m_root, m_dimensions.x, m_dimensions.y, m_dimensions.width, m_dimensions.height, &yCul);

	UpdateScrollbar();
}

bool TreeView::GetNodeVisible(TreeNode* pNode) const
{
	bool visible = true;

	if (pNode->m_pParentNode != nullptr)
	{
		if (pNode->m_pParentNode->m_expanded == false)
		{
			return false;
		}

		bool parentVisible = GetNodeVisible(pNode->m_pParentNode);

		return parentVisible;
	}

	return visible;
}

void TreeView::SetNodeVisible(TreeNode* pNode, bool visible) const
{
	pNode->m_pName->SetVisible(visible);
	pNode->m_pExpandButton->SetVisible(visible);

	if (pNode->m_vpChildren.size() == 0)
	{
		pNode->m_pExpandButton->SetVisible(false);
	}

	for (int i = 0; i < pNode->m_vpChildren.size(); ++i)
	{
		if (pNode->m_expanded && visible)
		{
			SetNodeVisible(pNode->m_vpChildren[i], true);
		}
		else
		{
			SetNodeVisible(pNode->m_vpChildren[i], false);
		}
	}
}

void TreeView::SetNodeDimensions(TreeNode* pNode, int x, int y, int width, int height, int* additionalYOffset)
{
	int expanderSpacer = 3;
	int yOffset = 4;
	int buttonSize = 10;

	pNode->m_pName->SetLocation(x + expanderSpacer + expanderSpacer + buttonSize, y);
	pNode->m_pExpandButton->SetDimensions(x + expanderSpacer, y + yOffset, buttonSize, buttonSize);

	pNode->m_x = x;
	pNode->m_y = y;
	pNode->m_width = width;
	pNode->m_height = height;

	// Go through and add the correct dimensions for the children and set their visibility
	x += m_xIndent;

	for (int i = 0; i < pNode->m_vpChildren.size(); ++i)
	{
		if (GetNodeVisible(pNode->m_vpChildren[i]))
		{
			y = *additionalYOffset;
			*additionalYOffset += m_yOffset;

			m_numOpenNodes += 1;
		}

		SetNodeDimensions(pNode->m_vpChildren[i], x, y, width, height, additionalYOffset);

		if (GetNodeVisible(pNode->m_vpChildren[i]))
		{
			SetNodeVisible(pNode->m_vpChildren[i], true);
		}
		else
		{
			SetNodeVisible(pNode->m_vpChildren[i], false);
		}

		if (pNode->m_expanded)
		{
			pNode->m_pExpandButton->SetLabelOffset(1, 2);
			pNode->m_pExpandButton->SetLabel("-");
		}
		else
		{
			pNode->m_pExpandButton->SetLabelOffset(0, 0);
			pNode->m_pExpandButton->SetLabel("+");
		}
	}
}

void TreeView::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	m_pBackgroundDefault->SetDimensions(0, -height, width, height);
	m_pBackgroundDisabled->SetDimensions(0, -height, width, height);

	m_scrollBar.SetDimensions(width, -height, 20, height);

	int yCul = m_yOffset;
	m_numOpenNodes = 0;
	SetNodeDimensions(m_root, 0, -20, width, height, &yCul);
}

void TreeView::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

ComponentType TreeView::GetComponentType() const
{
	return ComponentType::TreeView;
}

void TreeView::UpdateScrollbar()
{
	int visibleSize = GetDimensions().height;
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_GUIFont, "%s", "a");
	int yOffset = 4;

	int neededHeight = (m_numOpenNodes + 1) * (textHeight + yOffset);
	int heightDiff = neededHeight - visibleSize;

	float scrollRatio = m_scrollBar.GetScrollRatio();
	int offsetLabelY = static_cast<int>(heightDiff * scrollRatio);

	if (heightDiff > 0)
	{
		m_scrollBar.SetScrollSize(static_cast<float>(visibleSize) / static_cast<float>(neededHeight));
		m_scrollBar.SetDisabled(false);

		int yCul = -20 + m_yOffset + offsetLabelY;
		m_numOpenNodes = 0;
		SetNodeDimensions(m_root, 0, -20 + offsetLabelY, m_dimensions.width, m_dimensions.height, &yCul);
	}
	else
	{
		m_scrollBar.SetScrollSize(0.0f);
		m_scrollBar.SetDisabled(true);
	}
}

void TreeView::DrawChildren()
{
	// Do a scissor test here
	int scissorX = GetLocation().x - 1;
	int scissorY = GetLocation().y - 1;
	if (GetParent() != nullptr)
	{
		scissorX = GetParent()->GetLocation().x + scissorX;
		scissorY = GetParent()->GetLocation().y + scissorY;
	}

	int scissorWidth = GetDimensions().width + 2 + 20;
	int scissorHeight = GetDimensions().height + 2;

	scissorY = (scissorY - scissorHeight) + 2;

	bool isNeedScissor = true;

	if (isNeedScissor)
	{
		m_pRenderer->EnableScissorTest(scissorX, scissorY, scissorWidth, scissorHeight);
	}

	Container::DrawChildren();

	if (isNeedScissor)
	{
		m_pRenderer->DisableScissorTest();
	}
}

void TreeView::DrawSelf()
{
	m_pBackgroundDefault->SetVisible(false);
	m_pBackgroundDisabled->SetVisible(false);

	if (IsEnabled())
	{
		m_pBackgroundDefault->SetVisible(true);
	}
	else
	{
		m_pBackgroundDisabled->SetVisible(true);
	}

	m_scrollBar.SetVisible(true);
}

void TreeView::_ExpandPressed(void* pData)
{
	TreeNode* treeNode = static_cast<TreeNode*>(pData);

	if (treeNode)
	{
		treeNode->m_pTreeView->ExpandPressed(treeNode);
	}
}

void TreeView::ExpandPressed(TreeNode* pNode)
{
	if (pNode->m_expanded == false)
	{
		pNode->m_expanded = true;
	}
	else
	{
		pNode->m_expanded = false;
	}

	int yCul = -20 + m_yOffset;
	m_numOpenNodes = 0;
	SetNodeDimensions(m_root, 0, -20, m_dimensions.width, m_dimensions.height, &yCul);

	UpdateScrollbar();
}

void TreeView::_ScrollbarChanged(void* pData)
{
	TreeView* treeView = static_cast<TreeView*>(pData);
	treeView->ScrollbarChanged();
}

void TreeView::ScrollbarChanged()
{
	UpdateScrollbar();
}