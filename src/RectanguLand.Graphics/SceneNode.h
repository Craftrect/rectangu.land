#pragma once

namespace Graphics
{
	SHARED_FORWARD(SceneNode);

	class SceneNode
	{
	protected:
		std::list<SceneNodePtr> mOpaqueChildren;
		std::list<SceneNodePtr> mTransparentChildren;
		String mNodeName;

		virtual void renderChildren();
		virtual void updateChildren();
	public:
		SceneNode(const String& name);
		SceneNode();

		void setNodeName(const String& name) { mNodeName = name; }
		String getNodeName() const { return mNodeName; }

		virtual void render() = 0;
		virtual void update() = 0;
		virtual bool isTransparent() const { return false; }
		virtual float getDepth() const { return 0.0f; }

		virtual void addChild(SceneNodePtr child);
	};
}