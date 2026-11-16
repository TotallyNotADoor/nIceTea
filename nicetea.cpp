#include "nicetea.h"
#include "raylib.h"

vector<unsigned int> Keybinds;

vector<LevelHitbox> LevelGeometry;
vector<Trigger> LevelTriggers;

/******** LevelLoader ********/
bool LoadLevel(const string &path) {
	return false;
}
bool SaveLevel(const string &path) {
	return false;
}

/******** GameObject ********/
GameObject::GameObject() {
	Position = {0.0f, 0.0f};
}

/******** Sprite ********/
Sprite::Sprite() {
	Texture = { 0, 0, 0, 0, 0 };
	Asset = {nullptr, 0, 0, 0, 0};
	TextureDimetions = {0.0f, 0.0f};
}
Sprite::Sprite(const string& path) {
	Texture = { 0, 0, 0, 0, 0 };
	Asset = LoadImage(path.c_str());
	TextureDimetions = {static_cast<float>(Asset.width), static_cast<float>(Asset.height)};
	MakeTexture();
}
Sprite::Sprite(Image image) {
	Texture = { 0, 0, 0, 0, 0 };
	Asset = image;
	TextureDimetions = {static_cast<float>(Asset.width), static_cast<float>(Asset.height)};
	MakeTexture();
}
Sprite::Sprite(Texture2D texture) {
	Texture = texture;
	TextureDimetions = {static_cast<float>(Texture.width), static_cast<float>(Texture.height)};
	Asset = {nullptr, 0, 0, 0, 0};
}
Sprite::Sprite(Sprite&& other) noexcept : Texture(other.Texture), Asset(other.Asset) {
	other.Texture = {0, 0, 0, 0, 0};
	other.Asset = {nullptr, 0, 0, 0, 0};
}
Sprite& Sprite::operator=(Sprite&& other) noexcept {
	if (this != &other) {
		if (Asset.data != nullptr) UnloadImage(Asset);
		if (Texture.id > 0) UnloadTexture(Texture);
		Texture = other.Texture;
		Asset = other.Asset;
		other.Texture = {0, 0, 0, 0, 0};
		other.Asset = {nullptr, 0, 0, 0, 0};
	}
	return *this;
}
void Sprite::MakeTexture() {
		if(Texture.id > 0) UpdateTexture(Texture, Asset.data);
		else if(Asset.data != nullptr) Texture = LoadTextureFromImage(Asset);
}
void Sprite::SetTexture(Texture2D texture) {
	if(Texture.id > 0) UnloadTexture(Texture);
	Texture = texture;
}
void Sprite::Draw() const {
	if(Texture.id >0) DrawTexture(Texture, lround(Position.x), lround(Position.y), WHITE);
}
Sprite::~Sprite() {
	if(Asset.data != nullptr) UnloadImage(Asset);
	if(Texture.id > 0) UnloadTexture(Texture);
}

/******** AtlasSprite ********/
AtlasSprite::AtlasSprite() : Sprite(), Frame(UINT_NAN), Frames(UINT_NAN) {}
AtlasSprite::AtlasSprite(const string& path, unsigned int frames) : Sprite(path), Frames(frames), Frame(0) {
	if(frames != UINT_NAN) AtlasPos.resize(frames);
}
AtlasSprite::AtlasSprite(Image image, unsigned int frames) : Sprite(image), Frames(frames), Frame(0) {
	if(frames != UINT_NAN) AtlasPos.resize(frames);
}
AtlasSprite::AtlasSprite(Texture2D texture, unsigned int frames) : Sprite(texture), Frames(frames), Frame(0) {
	if(frames != UINT_NAN) AtlasPos.resize(frames);
}
void AtlasSprite::SetFrame(unsigned int frame) {
	Frame = frame%Frames;
}
void AtlasSprite::ChangeFrame(unsigned int change) {
	Frame += change;
	Frame = Frame%Frames;
}
void AtlasSprite::Draw() const {
	if(Texture.id > 0 && Frame != UINT_NAN) DrawTextureRec(Texture, AtlasPos[Frame], Position, WHITE);
}

/******** AnimatedSprite ********/
AnimatedSprite::AnimatedSprite() : AtlasSprite(), Timer(0), FramesPerFrame(UINT_NAN) {}
AnimatedSprite::AnimatedSprite(const string& path, unsigned int frames, unsigned int fpf) : AtlasSprite(path, frames), FramesPerFrame(fpf), Timer(0) {}
AnimatedSprite::AnimatedSprite(Image image, unsigned int frames, unsigned int fpf) : AtlasSprite(image, frames), FramesPerFrame(fpf), Timer(0) {}
AnimatedSprite::AnimatedSprite(Texture2D texture, unsigned int frames, unsigned int fpf) : AtlasSprite(texture, frames), FramesPerFrame(fpf), Timer(0) {}
void AnimatedSprite::SetTimer(unsigned int timer) {
	Timer = timer%FramesPerFrame;
}
void AnimatedSprite::ChangeTimer(unsigned int change) {
	Timer += change;
	Timer = Timer%FramesPerFrame;
}
void AnimatedSprite::UpdateAnimation() {
	if(Frame != UINT_NAN && Frames != UINT_NAN && FramesPerFrame != UINT_NAN) {
		Timer++;
		if(Timer >= FramesPerFrame) {
			Frame++;
			if(Frame >= Frames) Frame = 0;
			Timer = 0;
		}
	}
}

/******** AnimatedSpriteEx ********/

AnimatedSpriteEx::AnimatedSpriteEx() : AnimatedSprite() {
	SetAnimation(0);
}
AnimatedSpriteEx::AnimatedSpriteEx(const string& path, unsigned int number_of_animations) : AnimatedSprite(path, UINT_NAN, UINT_NAN) {
	AnimationSpan.resize(number_of_animations);
}
AnimatedSpriteEx::AnimatedSpriteEx(Image image, unsigned int number_of_animations) : AnimatedSprite(image, UINT_NAN, UINT_NAN) {
	AnimationSpan.resize(number_of_animations);
}
AnimatedSpriteEx::AnimatedSpriteEx(Texture2D texture, unsigned int number_of_animations) : AnimatedSprite(texture, UINT_NAN, UINT_NAN) {
	AnimationSpan.resize(number_of_animations);
}
void AnimatedSpriteEx::SetAnimation(unsigned int animation) {
	CurrentAnimation = animation;
	Frame = 0;
	Frames = (AnimationSpan[animation].EndPos - AnimationSpan[animation].StartPos) + 1;
	FramesPerFrame = AnimationSpan[animation].OneFrameLength;
	Timer = 0;
}
void AnimatedSpriteEx::ChangeAnimation(unsigned int change) {
	unsigned int animation = CurrentAnimation + change;
	animation = animation%AnimationSpan.size();
	SetAnimation(animation);
}
void AnimatedSpriteEx::Draw() const {
	if(Texture.id > 0 && Frame != UINT_NAN) DrawTextureRec(Texture, AtlasPos[Frame + AnimationSpan[CurrentAnimation].StartPos], Position, WHITE);
}

/******** PhysicsObject ********/
PhysicsObject::PhysicsObject() {
	Velocity = {0.0f, 0.0f};
	MaxVelocity = {10.0f, 10.0f};
	Gravity = {0.0f, 1.0f};
	GravityStrength = 1.0f;
	Friction = 0.5f;
	CurrentCollisionID = UINT_NAN;
}
PhysicsObject::PhysicsObject(float gravity_strength, float friction) {
	Velocity = {0.0f, 0.0f};
	MaxVelocity = {10.0f, 10.0f};
	Gravity = {0.0f, 1.0f};
	GravityStrength = gravity_strength;
	Friction = friction;
	CurrentCollisionID = UINT_NAN;
}
PhysicsObject::PhysicsObject(float gravity_strength, float friction, Vector2 max_velocity) {
	Velocity = {0.0f, 0.0f};
	MaxVelocity = max_velocity;
	Gravity = {0.0f, 1.0f};
	GravityStrength = gravity_strength;
	Friction = friction;
	CurrentCollisionID = UINT_NAN;
}
void PhysicsObject::ApplyFriction(bool affect_y) {
	if(affect_y) Velocity = Vector2Scale(Velocity, Friction);
	else Velocity.x = Velocity.x * Friction;
}
void PhysicsObject::ApplyForce(Vector2 force) {
	Velocity = Vector2Add(Velocity, force);
	Velocity = Vector2Clamp(Velocity, Vector2Negate(MaxVelocity), MaxVelocity);
}
bool PhysicsObject::IsCollidingWithLevel() {
	for(size_t i = 0; i < LevelGeometry.size(); ++i) {
		if(LevelGeometry[i].Active && CheckCollisionRecs(Hitbox, LevelGeometry[i].Hitbox)) {
				CurrentCollisionID = static_cast<unsigned int>(i);
				return true;
		}
	}
	return false;
}
bool PhysicsObject::IsCollidingWithCurrentCollider() {
	if(CurrentCollisionID != UINT_NAN && CurrentCollisionID < LevelGeometry.size()) {
		return CheckCollisionRecs(LevelGeometry[CurrentCollisionID].Hitbox, Hitbox);
	}
	return false;
}
void PhysicsObject::CheckTriggerCollisions() {
	CurrentTriggerCollisions.clear();
	for(Trigger i : LevelTriggers) {
		if(i.Active && CheckCollisionRecs(Hitbox, i.Hitbox)) CurrentTriggerCollisions.push_back(i.ID);
	}
}
void PhysicsObject::MoveAndCollide() {
	TouchingWall = false;
	Position.x += Velocity.x;
	Hitbox.x = Position.x;
	if(IsCollidingWithLevel()) {
		short direction = Sign(Velocity.x);;
		if(direction != 0) {
			while(IsCollidingWithCurrentCollider()) {
				Position.x -= direction;
				Hitbox.x = Position.x;
			}
		}
		Position.x = round(Position.x);
		Hitbox.x = Position.x;

		Hitbox.x += direction;
		if(IsCollidingWithCurrentCollider()) TouchingWall = true;
		Hitbox.x -= direction;

		Velocity.x = 0.0f;
		CurrentCollisionID = UINT_NAN;
	}
	Grounded = false;
	Position.y += Velocity.y;
	Hitbox.y = Position.y;
	if(IsCollidingWithLevel()) {
		short direction = Sign(Velocity.y);
		if(direction != 0) {
			while(IsCollidingWithCurrentCollider()) {
				Position.y -= direction;
				Hitbox.y = Position.y;
			}
		}
		Position.y = round(Position.y);
		Hitbox.y = Position.y;

		Hitbox.y += 1.0f;
		if(IsCollidingWithCurrentCollider()) Grounded = true;
		Hitbox.y -= 1.0f;

		Velocity.y = 0.0f;
		CurrentCollisionID = UINT_NAN;

	}
}

/******** PhysicsEntity ********/
	PhysicsEntity::PhysicsEntity(float gravity_strength, float friction, Vector2 max_velocity, const string& path, unsigned int number_of_animations)
	: PhysicsObject(gravity_strength, friction, max_velocity), AnimatedSpriteEx(path, number_of_animations) {}

	PhysicsEntity::PhysicsEntity(float gravity_strength, float friction, Vector2 max_velocity, Image image, unsigned int number_of_animations)
	: PhysicsObject(gravity_strength, friction, max_velocity), AnimatedSpriteEx(image, number_of_animations) {}

	PhysicsEntity::PhysicsEntity(float gravity_strength, float friction, Vector2 max_velocity, Texture2D texture, unsigned int number_of_animations)
	: PhysicsObject(gravity_strength, friction, max_velocity), AnimatedSpriteEx(texture, number_of_animations) {}

/******** Standalone Functions ********/
bool IsRectVisibleOnScreen(Rectangle rect, Camera2D camera) {
	Vector2 topLeft = GetScreenToWorld2D((Vector2){ 0, 0 }, camera);
    Vector2 bottomRight = GetScreenToWorld2D(
        (Vector2){ (float)GetScreenWidth(), (float)GetScreenHeight() },
        camera
    );

    Rectangle view = {
        topLeft.x,
        topLeft.y,
        bottomRight.x - topLeft.x,
        bottomRight.y - topLeft.y
    };

    return CheckCollisionRecs(rect, view);
}
bool IsSpriteVisibleOnScreen(Sprite sprite, Camera2D camera) {
	Vector2 topLeft = GetScreenToWorld2D((Vector2){ 0, 0 }, camera);
    Vector2 bottomRight = GetScreenToWorld2D(
        (Vector2){ (float)GetScreenWidth(), (float)GetScreenHeight() },
        camera
    );

    Rectangle view = {
        topLeft.x,
        topLeft.y,
        bottomRight.x - topLeft.x,
        bottomRight.y - topLeft.y
    };

    return CheckCollisionRecs({sprite.Position.x, sprite.Position.y, sprite.TextureDimetions.x, sprite.TextureDimetions.y}, view);
}
