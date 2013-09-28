#pragma once

namespace Graphics
{
	class Camera
	{
	protected:
		Utils::Matrix mView;
		Utils::Matrix mProjection;
		Utils::Matrix mInvView;
		Utils::Matrix mInvProj;

		Utils::Vector3 mPosition;
		Utils::Vector3 mTarget;
		Utils::Vector3 mUp;
		Utils::Vector3 mRight;

		Camera();

		virtual void updateView();
		virtual void projChanged();
	public:
		virtual void setPosition(const Utils::Vector3& position);
		virtual void setTarget(const Utils::Vector3& target);
		virtual void yaw(float value);
		virtual void pitch(float value);
		virtual void roll(float value);
		virtual void move(const Utils::Vector3& direction, float amount);
		virtual Utils::Vector3 getForward() const { return (mTarget - mPosition).normalized(); }
		virtual Utils::Vector3 getPosition() const { return mPosition; }
		virtual Utils::Vector3 getRight() const { return mRight; }
		virtual Utils::Vector3 getUp() const { return mUp; }

		Utils::Event<void (const Utils::Matrix&)> ViewChanged;
		Utils::Event<void (const Utils::Matrix&)> ProjChanged;

		virtual const Utils::Matrix& getView() const { return mView; }
		virtual const Utils::Matrix& getProjection() const { return mProjection; }
		virtual const Utils::Matrix& getInvView() const { return mInvView; }
		virtual const Utils::Matrix& getInvProj() const { return mInvProj; }
	};

	SHARED_TYPE(Camera);
}