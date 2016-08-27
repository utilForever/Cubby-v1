/*************************************************************************
> File Name: TreeView.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A simple GUI component that simple displays a text label.
> Created Time: 2016/08/27
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_TREE_VIEW_H
#define CUBBY_TREE_VIEW_H

#include "Button.h"
#include "Label.h"
#include "ScrollBar.h"

// Forward declaration
struct TreeNode;
class TreeView;

using TreeNodeList = std::vector<TreeNode*>;

struct TreeNode
{
	TreeView* m_pTreeView;

	Label* m_pName;
	Button* m_pExpandButton;

	bool m_expanded;
	int m_x;
	int m_y;
	int m_width;
	int m_height;

	TreeNode* m_pParentNode;
	TreeNodeList m_vpChildren;
};

class TreeView : public Container
{
public:
	// Constructor, Destructor
	TreeView(Renderer* pRenderer, unsigned int GUIFont, std::string rootName);
	~TreeView();

	void ClearTreeView();
	void ClearNode(TreeNode* pNode);

	void SetRootName(std::string rootName) const;

	TreeNode* FindNode(std::string name, TreeNode* pRoot) const;

	void AddRootNode(std::string name);
	void AddChildNode(std::string parentName, std::string name);

	bool GetNodeVisible(TreeNode* pNode) const;
	void SetNodeVisible(TreeNode* pNode, bool visible) const;
	void SetNodeDimensions(TreeNode* pNode, int x, int y, int width, int height, int* additionalYOffset);

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	ComponentType GetComponentType() const override;

	void UpdateScrollbar();

protected:
	void DrawChildren() override;
	void DrawSelf() override;

private:
	static void _ExpandPressed(void* pData);
	void ExpandPressed(TreeNode* pNode);

	static void _ScrollbarChanged(void* pData);
	void ScrollbarChanged();

private:
	Renderer* m_pRenderer;
	unsigned int m_GUIFont;

	int m_xIndent;
	int m_yOffset;

	RenderRectangle* m_pBackgroundDefault;
	RenderRectangle* m_pBackgroundDisabled;

	ScrollBar m_scrollBar;

	int m_numOpenNodes;

	TreeNode* m_root;
};

#endif