#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <string>
#include <cmath>
#include <climits>
using namespace std;

constexpr unsigned int UINT_NAN = UINT_MAX;

inline vector<unsigned int> Keybinds;

struct LevelHitbox {
	Rectangle Hitbox;
	bool Active;
};
struct Trigger {
	unsigned int ID;
    Rectangle Hitbox;
    bool Active;
};

inline vector<Trigger> LevelTriggers;
inline vector<LevelHitbox> LevelGeometry;

bool LoadLevel(const string& path);
bool SaveLevel(const string& path);

template<typename Type>
short Sign(Type val) noexcept {
    return (Type(0) < val) - (val < Type(0));
}

class GameObject {
	public:
		Vector2 Position;

		GameObject();
};

class Sprite : public virtual GameObject {
	protected:
		Texture2D Texture;
	public:
		Image Asset;
		Vector2 TextureDimetions;

		Sprite();
		explicit Sprite(const string& path);
		explicit Sprite(Image image);
		explicit Sprite(Texture2D texture);

		Sprite(Sprite&& other) noexcept;
		Sprite& operator=(Sprite&& other) noexcept;

		Sprite(const Sprite&) = delete;
		Sprite& operator=(const Sprite&) = delete;

		void MakeTexture();
		void SetTexture(Texture2D texture);
		virtual void Draw() const;

		virtual ~Sprite();
};

struct LevelSprite {
	Sprite Visual;
	bool Active;

	explicit LevelSprite(Texture2D texture) : Visual(texture), Active(true){};
};
inline vector<LevelSprite> LevelVisuals;

class AtlasSprite : public Sprite {
	protected:
		unsigned int Frame;
		unsigned int Frames;
	public:
		vector<Rectangle> AtlasPos;

		AtlasSprite();
		AtlasSprite(const string& path, unsigned int frames);
		AtlasSprite(Image image, unsigned int frames);
		AtlasSprite(Texture2D texture, unsigned int frames);

		void SetFrame(unsigned int frame);
		void ChangeFrame(unsigned int change);

		virtual void Draw() const override;
};

class AnimatedSprite : public AtlasSprite {
	protected:
		unsigned int Timer;
	public:
		unsigned int FramesPerFrame;

		AnimatedSprite();
		AnimatedSprite(const string& path, unsigned int frames, unsigned int fpf);
		AnimatedSprite(Image image, unsigned int frames, unsigned int fpf);
		AnimatedSprite(Texture2D texture, unsigned int frames, unsigned int fpf);

		void SetTimer(unsigned int timer);
		void ChangeTimer(unsigned int change);
		void UpdateAnimation();
};

class AnimatedSpriteEx : public AnimatedSprite {
	protected:
		struct AnimationData {
			unsigned int StartPos;
			unsigned int EndPos;
			unsigned int OneFrameLength;
		};
		unsigned int CurrentAnimation;
	public:
		vector<AnimationData> AnimationSpan;
		AnimatedSpriteEx();
		AnimatedSpriteEx(const string& path, unsigned int number_of_animations);
		AnimatedSpriteEx(Image image, unsigned int number_of_animations);
		AnimatedSpriteEx(Texture2D texture, unsigned int number_of_animations);

		void SetAnimation(unsigned int animation);
		void ChangeAnimation(unsigned int change);

		void Draw() const override;
};

class PhysicsObject : public virtual GameObject {
	protected:
		bool IsCollidingWithCurrentCollider();
	public:
		Rectangle Hitbox;
		Vector2 Velocity;
		Vector2 MaxVelocity;
		Vector2 Gravity;
		float Friction;
		float GravityStrength;
		vector<unsigned int>CurrentTriggerCollisions;
		unsigned int CurrentCollisionID;
		bool TouchingWall;
		bool Grounded;
		PhysicsObject();
		PhysicsObject(float gravity_strength, float friction);
		PhysicsObject(float gravity_strength, float friction, Vector2 max_velocity);

		void ApplyFriction(bool affect_y);
		void ApplyForce(Vector2 force);

		void CheckTriggerCollisions();
		bool IsCollidingWithLevel();
		void MoveAndCollide();
};

class PhysicsEntity : public PhysicsObject, public AnimatedSpriteEx {
	public:
		PhysicsEntity(float gravity_strength, float friction, Vector2 max_velocity, const string& path, unsigned int number_of_animations);
		PhysicsEntity(float gravity_strength, float friction, Vector2 max_velocity, Image image, unsigned int number_of_animations);
		PhysicsEntity(float gravity_strength, float friction, Vector2 max_velocity, Texture2D texture, unsigned int number_of_animations);
};

bool IsRectVisibleOnScreen(Rectangle rect, Camera2D camera);
bool IsSpriteVisibleOnScreen(const Sprite& sprite, Camera2D camera);
