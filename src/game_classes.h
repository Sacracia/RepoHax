#pragma once

#include "haxsdk/unity/hax_unity.h"

struct PrefabRef : System::Object
{
    META("Assembly-CSharp", "", "PrefabRef");

    PrefabRef() : System::Object(nullptr) {}
    PrefabRef(UVM::Object* ptr) : System::Object(ptr) {}

    inline operator bool() const { return !null(); }

    UnityEngine::GameObject Prefab() { THROW_IF_NULL(); return s_get_Prefab.CallThunk<UnityEngine::GameObject, PrefabRef>(*this); }

    FIELD(prefabName, System::String);
    FIELD(resourcePath, System::String);

private:
    METHOD(get_Prefab);
};

struct MenuCursor : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "MenuCursor");

    MenuCursor() : UnityEngine::MonoBehaviour(nullptr) {}
    MenuCursor(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(showTimer, float);

    METHOD(Update);
};

struct PlayerHealth : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "PlayerHealth");

    PlayerHealth() : UnityEngine::MonoBehaviour(nullptr) {}
    PlayerHealth(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    void HealOther(int healAmount, bool effect)
    {
        THROW_IF_NULL();
        s_HealOther.CallThunk<void, PlayerHealth, int, bool>(*this, healAmount, effect);
    }

    FIELD(maxHealth, int);
    FIELD(health, int);
    FIELD(renderers, System::List<UnityEngine::MeshRenderer>);

    METHOD(Hurt);
    METHOD(Update);
private:
    METHOD(HealOther);
};

struct PlayerDeathHead;
struct PhysGrabber;
struct PlayerVoiceChat;
struct PlayerTumble;
struct MapToolController;
struct PlayerAvatarVisuals;
struct PlayerAvatar : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "PlayerAvatar");

    PlayerAvatar() : UnityEngine::MonoBehaviour(nullptr) {}
    PlayerAvatar(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() { return !null(); }
    inline bool operator==(PlayerAvatar o) const { return GetPtr() == o.GetPtr(); }

    void PlayerDeath(int enemyIndex)
    {
        THROW_IF_NULL();
        s_PlayerDeath.Invoke<void, PlayerAvatar, int>(*this, enemyIndex);
    }

    void Revive(bool revivedByTruck = false)
    {
        THROW_IF_NULL();
        s_Revive.Invoke<void, PlayerAvatar, bool>(*this, revivedByTruck);
    }

    STATIC_FIELD(instance, PlayerAvatar);
    FIELD(playerHealth, PlayerHealth);
    FIELD(isLocal, bool);
    FIELD(deadSet, bool);
    FIELD(playerDeathHead, PlayerDeathHead);
    FIELD(physGrabber, PhysGrabber);
    FIELD(playerName, System::String);
    FIELD(voiceChat, PlayerVoiceChat);
    FIELD(spawned, bool);
    FIELD(photonView, Photon::PhotonView);
    FIELD(tumble, PlayerTumble);
    FIELD(upgradeMapPlayerCount, int);
    FIELD(isCrouching, bool);
    FIELD(isDisabled, bool);
    FIELD(spectatePoint, UnityEngine::Transform);
    FIELD(mapToolController, MapToolController);
    FIELD(playerTransform, UnityEngine::Transform);
    FIELD(playerAvatarVisuals, PlayerAvatarVisuals);
    FIELD(voiceChatFetched, bool);

    METHOD(OnDestroy);
    METHOD(PlayerDeath);
    METHOD(Revive);
};

struct PlayerVoiceChat : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "PlayerVoiceChat");

    PlayerVoiceChat() : UnityEngine::MonoBehaviour(nullptr) {}
    PlayerVoiceChat(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(inLobbyMixer, bool);

    void ToggleMixer(bool toggle, bool distorted)
    {
        THROW_IF_NULL();
        s_ToggleMixer.CallThunk<void, PlayerVoiceChat, bool, bool>(*this, toggle, distorted);
    }

private:
    METHOD(ToggleMixer);
};

struct PlayerController : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "PlayerController");

    PlayerController() : UnityEngine::MonoBehaviour(nullptr) {}
    PlayerController(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() { return !null(); }

    STATIC_FIELD(instance, PlayerController);
    FIELD(EnergyCurrent, float);
    FIELD(EnergyStart, float);
    FIELD(MoveSpeed, float);
    FIELD(SprintSpeed, float);
    FIELD(CrouchSpeed, float);
    FIELD(JumpExtra, int);
    FIELD(SprintSpeedUpgrades, float);

    METHOD(FixedUpdate);
    METHOD(Update);
};

struct PlayerTumble : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "PlayerTumble");

    PlayerTumble() : UnityEngine::MonoBehaviour(nullptr) {}
    PlayerTumble(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() { return !null(); }

    void TumbleRequest(bool _isTumbling, bool _playerInput) { THROW_IF_NULL(); s_TumbleRequest.CallThunk<void, PlayerTumble, bool, bool>(*this, _isTumbling, _playerInput); }

    FIELD(tumbleLaunch, int);
    FIELD(playerAvatar, PlayerAvatar);

    METHOD(TumbleRequest);
};

struct FlashlightController : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "FlashlightController");

    FlashlightController() : UnityEngine::MonoBehaviour(nullptr) {}
    FlashlightController(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() { return !null(); }

    FIELD(spotlight, UnityEngine::Light);
    FIELD(baseIntensity, float);

    METHOD(Update);
};

struct ItemToggle : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "ItemToggle");

    inline operator bool() const { return !null(); }

    void ToggleItem(bool toggle, int player = -1)
    {
        THROW_IF_NULL();
        s_ToggleItem.CallThunk<void, ItemToggle, bool, int>(*this, toggle, player);
    }

    FIELD(toggleState, bool);
    FIELD(photonView, Photon::PhotonView);

private:
    METHOD(ToggleItem);
};

struct ItemAttributes : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "ItemAttributes");

    ItemAttributes() : UnityEngine::MonoBehaviour(nullptr) {}
    ItemAttributes(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(itemAssetName, System::String);
};

struct ItemUpgrade : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "ItemUpgrade");

    ItemUpgrade() = default;
    ItemUpgrade(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(upgradeDone, bool);
    FIELD(itemToggle, ItemToggle);
    FIELD(itemAttributes, ItemAttributes);
    FIELD(isPlayerUpgrade, bool);
    FIELD(photonView, Photon::PhotonView);

    METHOD(Update);
};

struct ItemHealthPack : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "ItemHealthPack");

    ItemHealthPack() = default;
    ItemHealthPack(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(used, bool);
    FIELD(itemToggle, ItemToggle);
    FIELD(photonView, Photon::PhotonView);
    FIELD(itemAttributes, ItemAttributes);

    METHOD(Update);
};

struct SemiFunc
{
    META("Assembly-CSharp", "", "SemiFunc");

    static inline UnityEngine::Camera MainCamera() 
    { 
        return s_MainCamera.CallThunk<UnityEngine::Camera>(); 
    }

    static inline bool OnScreen(const UnityEngine::Vector3& pos, float paddWidth, float paddHeight) 
    { 
        return s_OnScreen.CallThunk<bool, const System::Boxed<UnityEngine::Vector3>&, float, float>(System::Boxed<UnityEngine::Vector3>(pos), paddWidth, paddHeight); 
    }

    static inline bool IsMasterClientOrSingleplayer()
    {
        return s_IsMasterClientOrSingleplayer.CallThunk<bool>();
    }

    static inline bool IsMultiplayer()
    {
        return s_IsMultiplayer.CallThunk<bool>();
    }

    static inline bool IsMasterClient()
    {
        return s_IsMasterClient.CallThunk<bool>();
    }

    static inline System::String PlayerGetSteamID(PlayerAvatar player)
    {
        return s_PlayerGetSteamID.CallThunk<System::String, PlayerAvatar>(player);
    }

    static inline int StatSetRunCurrency(int value)
    {
        return s_StatSetRunCurrency.CallThunk<int, int>(value);
    }

    static inline int PhotonViewIDPlayerAvatarLocal()
    {
        return s_PhotonViewIDPlayerAvatarLocal.CallThunk<int>();
    }

    static inline int LayerMaskGetVisionObstruct()
    {
        return s_LayerMaskGetVisionObstruct.CallThunk<System::Boxed<UnityEngine::LayerMask>*>()->m_Value.m_Mask;
    }

    static inline System::String PlayerGetName(PlayerAvatar player)
    {
        return s_PlayerGetName.CallThunk<System::String, PlayerAvatar>(player);
    }

private:
    METHOD(OnScreen);
    METHOD(MainCamera);
    METHOD(IsMasterClientOrSingleplayer);
    METHOD(IsMultiplayer);
    METHOD(IsMasterClient);
    METHOD(PlayerGetSteamID);
    METHOD(StatSetRunCurrency);
    METHOD(PhotonViewIDPlayerAvatarLocal);
    METHOD(LayerMaskGetVisionObstruct);
    METHOD(PlayerGetName);
};

struct PhysGrabObjectImpactDetector;
struct PhysGrabObject : UnityEngine::Behaviour
{
    META("Assembly-CSharp", "", "PhysGrabObject");

    PhysGrabObject() : UnityEngine::Behaviour(nullptr) {}
    PhysGrabObject(UVM::Object* ptr) : UnityEngine::Behaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(grabbedLocal, bool);
    FIELD(centerPoint, UnityEngine::Vector3);
    FIELD(boundingBox, UnityEngine::Vector3);
    FIELD(midPoint, UnityEngine::Vector3);
    FIELD(impactDetector, PhysGrabObjectImpactDetector);

    METHOD(PhysicsGrabbingManipulation);
};

struct PhysGrabber : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "PhysGrabber");

    PhysGrabber() : UnityEngine::MonoBehaviour(nullptr) {}
    PhysGrabber(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    void PhysGrabOverChargeImpact()
    {
        THROW_IF_NULL();
        s_PhysGrabOverChargeImpact.CallThunk<void, PhysGrabber>(*this);
    }

    FIELD(grabRange, float);
    FIELD(grabStrength, float);
    FIELD(throwStrength, float);

    METHOD(PhysGrabLogic);
    METHOD(RayCheck);
    METHOD(PhysGrabOverChargeImpact);
    METHOD(PhysGrabOverCharge);
};

struct LevelValuables : System::Object
{
    META("Assembly-CSharp", "", "LevelValuables");

    LevelValuables() : System::Object(nullptr) {}
    LevelValuables(UVM::Object* ptr) : System::Object(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD_EX(tinyList, "tiny", System::List<PrefabRef>);
    FIELD_EX(smallList, "small", System::List<PrefabRef>);
    FIELD_EX(mediumList, "medium", System::List<PrefabRef>);
    FIELD_EX(bigList, "big", System::List<PrefabRef>);
    FIELD_EX(wideList, "wide", System::List<PrefabRef>);
    FIELD_EX(tallList, "tall", System::List<PrefabRef>);
    FIELD_EX(veryTallList, "veryTall", System::List<PrefabRef>);
};

struct Level : UnityEngine::Object
{
    META("Assembly-CSharp", "", "Level");

    Level() : UnityEngine::Object(nullptr) {}
    Level(UVM::Object* ptr) : UnityEngine::Object(ptr) {}

    inline operator bool() const { return !null(); }
    inline bool operator==(const Level& o) const { return GetPtr() == o.GetPtr(); }

    FIELD(NarrativeName, System::String);
    FIELD(ValuablePresets, System::List<LevelValuables>);
};

struct RunManager : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "RunManager");

    inline operator bool() const { return !null(); }

    void ChangeLevel(bool completedLevel, bool levelFailed, int changeLevelType = 0)
    {
        THROW_IF_NULL();
        s_ChangeLevel.CallThunk<void, RunManager, bool, bool, int>(*this, completedLevel, levelFailed, changeLevelType);
    }

    STATIC_FIELD(instance, RunManager);
    FIELD(levels, System::List<Level>);
    FIELD(levelCurrent, Level);
    FIELD(levelMainMenu, Level);
    FIELD(levelLobbyMenu, Level);
    FIELD(levelShop, Level);
    FIELD(previousRunLevel, Level);
    FIELD(levelsCompleted, int);
    FIELD(levelIsShop, bool);

    METHOD(ChangeLevel);
    METHOD(SetRunLevel);
};

struct GameDirector : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "GameDirector");

    inline operator bool() { return !null(); }

    STATIC_FIELD(instance, GameDirector);
    FIELD(MainCamera, UnityEngine::Camera);
    FIELD(currentState, int);
    FIELD(PlayerList, System::List<PlayerAvatar>);
};

struct GameDirector_gameState
{
    META("Assembly-CSharp", "", "GameDirector/gameState");

    ENUM_VALUE(Main);
};

struct SpectateCamera : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "SpectateCamera");

    SpectateCamera() : UnityEngine::MonoBehaviour(nullptr) {}
    SpectateCamera(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() { return !null(); }

    STATIC_FIELD(instance, SpectateCamera);
    FIELD(headEnergy, float);
    FIELD(headEnergyEnough, bool);    

    METHOD(HeadEnergyLogic);
};

struct PlayerHealthGrab : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "PlayerHealthGrab");

    PlayerHealthGrab() : UnityEngine::MonoBehaviour(nullptr) {}
    PlayerHealthGrab(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() { return !null(); }
};

struct PlayerAvatarVisuals : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "PlayerAvatarVisuals");

    PlayerAvatarVisuals() : UnityEngine::MonoBehaviour(nullptr) {}
    PlayerAvatarVisuals(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() { return !null(); }

    FIELD(playerAvatar, PlayerAvatar);
    FIELD(isMenuAvatar, bool);
    FIELD(meshParent, UnityEngine::GameObject);
    FIELD(animator, UnityEngine::Behaviour);
    FIELD(headSideTransform, UnityEngine::Transform);
    FIELD(legTwistTransform, UnityEngine::Transform);
    FIELD(showSelfOverrideTimer, float);

    METHOD(Update);
};

struct GameManager : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "GameManager");

    GameManager() : UnityEngine::MonoBehaviour(nullptr) {}
    GameManager(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() { return !null(); }

    STATIC_FIELD(instance, GameManager);
    PROPERTY(gameMode, int);
};

struct MapToolController : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "MapToolController");

    MapToolController() : UnityEngine::MonoBehaviour(nullptr) {}
    MapToolController(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() { return !null(); }

    FIELD(Active, bool);
};

struct EnemyRigidbody : UnityEngine::Behaviour
{
    META("Assembly-CSharp", "", "EnemyRigidbody");

    EnemyRigidbody() : UnityEngine::Behaviour(nullptr) {}
    EnemyRigidbody(UVM::Object* ptr) : UnityEngine::Behaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(physGrabObject, PhysGrabObject);
    FIELD(grabShakeReleaseTimer, float);
    FIELD(grabStrengthTimer, float);
    FIELD(grabStrengthTime, float);
    FIELD(grabTimeCurrent, float);

    METHOD(FixedUpdate);
};

struct EnemyHealth : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "EnemyHealth");

    EnemyHealth() : UnityEngine::MonoBehaviour(nullptr) {}
    EnemyHealth(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    void Hurt(int damage, const UnityEngine::Vector3& hurtDirection)
    {
        THROW_IF_NULL();
        s_Hurt.CallThunk<void, EnemyHealth, int, const System::Boxed<UnityEngine::Vector3>&>(*this, damage, System::Boxed<UnityEngine::Vector3>(hurtDirection));
    }

    FIELD(healthCurrent, int);
    FIELD(health, int);
    FIELD(dead, bool);

    METHOD(Hurt);
};

struct EnemyState
{
    META("Assembly-CSharp", "", "EnemyState");

    ENUM_VALUE(None);
    ENUM_VALUE(Spawn);
    ENUM_VALUE(Despawn);
};

struct EnemyParent;
struct Enemy : UnityEngine::Behaviour
{
    META("Assembly-CSharp", "", "Enemy");

    Enemy() : UnityEngine::Behaviour(nullptr) {}
    Enemy(UVM::Object* ptr) : UnityEngine::Behaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(Rigidbody, EnemyRigidbody);
    FIELD(CenterTransform, UnityEngine::Transform);
    FIELD(Health, EnemyHealth);
    FIELD(CurrentState, int);
    FIELD_EX(enemyParent, "EnemyParent", EnemyParent);

    METHOD(Update);
};

struct EnemyParent : UnityEngine::Behaviour
{
    META("Assembly-CSharp", "", "EnemyParent");

    EnemyParent() : UnityEngine::Behaviour(nullptr) {}
    explicit EnemyParent(UVM::Object* ptr) : UnityEngine::Behaviour(ptr) {}

    inline operator bool() const { return !null(); }
    inline bool operator==(const EnemyParent& e) const { return GetPtr() == e.GetPtr(); }

    FIELD_EX(enemy, "Enemy", Enemy);
    FIELD(Spawned, bool);
    FIELD(enemyName, System::String);
};

struct EnemySetup : UnityEngine::Object
{
    META("Assembly-CSharp", "", "EnemySetup");

    EnemySetup() : UnityEngine::Object(nullptr) {}
    EnemySetup(UVM::Object* ptr) : UnityEngine::Object(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(spawnObjects, System::List<PrefabRef>);
};

struct EnemyDirector : UnityEngine::Behaviour
{
    META("Assembly-CSharp", "", "EnemyDirector");

    EnemyDirector() : UnityEngine::Behaviour(nullptr) {}
    EnemyDirector(UVM::Object* ptr) : UnityEngine::Behaviour(ptr) {}

    inline operator bool() const { return !null(); }

    STATIC_FIELD(instance, EnemyDirector);
    FIELD(enemiesSpawned, System::List<EnemyParent>);
    FIELD(enemiesDifficulty1, System::List<EnemySetup>);
    FIELD(enemiesDifficulty2, System::List<EnemySetup>);
    FIELD(enemiesDifficulty3, System::List<EnemySetup>);
    FIELD(debugNoVision, bool);
    FIELD(debugNoGrabMaxTime, bool);
    FIELD(debugEasyGrab, bool);
};

struct RoomVolume : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "RoomVolume");

    RoomVolume() : UnityEngine::MonoBehaviour(nullptr) {}
    RoomVolume(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    void SetExplored()
    {
        THROW_IF_NULL();
        s_SetExplored.CallThunk<void, RoomVolume>(*this);
    }

private:
    METHOD(SetExplored);
};

struct LevelPoint : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "LevelPoint");

    LevelPoint() : UnityEngine::MonoBehaviour(nullptr) {}
    LevelPoint(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(Room, RoomVolume);
};

struct LevelGenerator : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "LevelGenerator");

    LevelGenerator() : UnityEngine::MonoBehaviour(nullptr) {}
    LevelGenerator(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    void EnemySpawn(EnemySetup enemySetup, UnityEngine::Vector3 position)
    {
        THROW_IF_NULL();
        s_EnemySpawn.CallThunk<void, LevelGenerator, EnemySetup, const System::Boxed<UnityEngine::Vector3>&>(*this, enemySetup, System::Boxed<UnityEngine::Vector3>(position));
    }

    STATIC_FIELD(Instance, LevelGenerator);
    FIELD(LevelPathPoints, System::List<LevelPoint>);
    FIELD_EX(currentLevel, "Level", Level);
    FIELD(Generated, bool);

private:
    static inline System::MethodInfo s_EnemySpawn{{"Assembly-CSharp", "", "LevelGenerator"}, "EnemySpawn"};
};

struct EnemySlowMouth : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "EnemySlowMouth");

    EnemySlowMouth() : UnityEngine::MonoBehaviour(nullptr) {}
    EnemySlowMouth(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() { return !null(); }

    FIELD(attachedTimer, float);
};

struct MenuManager : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "MenuManager");

    MenuManager() : UnityEngine::MonoBehaviour(nullptr) {}
    MenuManager(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    STATIC_FIELD(instance, MenuManager);
    FIELD(currentMenuPage, Component);
};

struct PhysGrabObjectImpactDetector : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "PhysGrabObjectImpactDetector");

    PhysGrabObjectImpactDetector() : UnityEngine::MonoBehaviour(nullptr) {}
    PhysGrabObjectImpactDetector(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    void HealLogic(float healAmount, const UnityEngine::Vector3& healingPoint)
    {
        s_HealLogic.CallThunk<void, PhysGrabObjectImpactDetector, float, const System::Boxed<UnityEngine::Vector3>&>(*this, healAmount, System::Boxed<UnityEngine::Vector3>(healingPoint));
    }

    FIELD(isValuable, bool);
    FIELD(impactDisabledTimer, float);
    FIELD(photonView, Photon::PhotonView);

    METHOD(FixedUpdate);
    METHOD(Break);
    METHOD(DestroyObject);
private:
    METHOD(HealLogic);
};

struct ItemBattery : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "ItemBattery");

    ItemBattery() : UnityEngine::MonoBehaviour(nullptr) {}
    ItemBattery(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    void SetBatteryLife(int batteryLife)
    {
        THROW_IF_NULL();
        s_SetBatteryLife.CallThunk<void, ItemBattery, int>(*this, batteryLife);
    }

    void BatteryFullPercentChange(int batteryLifeInt, bool charge = false)
    {
        THROW_IF_NULL();
        s_BatteryFullPercentChange.CallThunk<void, ItemBattery, int, bool>(*this, batteryLifeInt, charge);
    }

    FIELD(batteryLife, float);
    FIELD(batteryLifeInt, int);
    FIELD(batteryBars, int);
    FIELD(physGrabObject, PhysGrabObject);

    METHOD(Update);
    METHOD(SetBatteryLife);
    METHOD(BatteryFullPercentChange);
};

struct ValuableObject : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "ValuableObject");

    ValuableObject() : UnityEngine::MonoBehaviour(nullptr) {}
    ValuableObject(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    void Discover(int state)
    {
        THROW_IF_NULL();
        s_Discover.CallThunk<void, ValuableObject, int>(*this, state);
    }

    FIELD(physGrabObject, PhysGrabObject);
    FIELD(dollarValueCurrent, float);
    FIELD(dollarValueOverride, int);

private:
    METHOD(Discover);
};

struct CosmeticWorldObjectSetup : System::Object
{
    META("Assembly-CSharp", "", "ValuableDirector/CosmeticWorldObjectSetup");

    CosmeticWorldObjectSetup() : System::Object(nullptr) {}
    CosmeticWorldObjectSetup(UVM::Object* ptr) : System::Object(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(prefab, PrefabRef);
};

struct ValuableDirector : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "ValuableDirector");

    ValuableDirector() : UnityEngine::MonoBehaviour(nullptr) {}
    ValuableDirector(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    STATIC_FIELD(instance, ValuableDirector);
    FIELD(valuableList, System::List<ValuableObject>);
    FIELD(cosmeticWorldObjectSetups, System::List<CosmeticWorldObjectSetup>);
    FIELD(cosmeticWorldObjectsLevelLoopsMax, int);

    METHOD(CosmeticWorldObjectLevelLoopsClampedGet);
};

struct ItemGun : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "ItemGun");

    ItemGun() : UnityEngine::MonoBehaviour(nullptr) {}
    ItemGun(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(gunRandomSpread, float);
    FIELD(physGrabObject, PhysGrabObject);
    FIELD(gunMuzzle, UnityEngine::Transform);
    FIELD(gunRange, float);

    METHOD(Shoot);
    METHOD(Update);
};

struct Item : UnityEngine::Object
{
    META("Assembly-CSharp", "", "Item");

    Item() : UnityEngine::Object(nullptr) {}
    Item(UVM::Object* ptr) : UnityEngine::Object(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(prefab, PrefabRef);
};

struct StatsManager : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "StatsManager");

    inline operator bool() const { return !null(); }

    STATIC_FIELD(instance, StatsManager);
    FIELD(itemDictionary, System::Dictionary<System::String COMMA Item>);
};

struct ExtractionPoint : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "ExtractionPoint");

    ExtractionPoint() : UnityEngine::MonoBehaviour(nullptr) {}
    ExtractionPoint(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }
    inline bool operator==(const ExtractionPoint& o) const { return GetPtr() == o.GetPtr(); }

    FIELD(isLocked, bool);
    FIELD(currentState, int);

    void HaulGoalSet(int value)
    {
        THROW_IF_NULL();
        s_HaulGoalSet.CallThunk<void, ExtractionPoint, int>(*this, value);
    }

    void OnClick()
    {
        THROW_IF_NULL();
        s_OnClick.CallThunk<void, ExtractionPoint>(*this);
    }

private:
    METHOD(HaulGoalSet);
    METHOD(OnClick);
};

struct CosmeticWorldObject : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "CosmeticWorldObject");

    CosmeticWorldObject() : UnityEngine::MonoBehaviour(nullptr) {}
    CosmeticWorldObject(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(rarity, int);
};

struct RoundDirector : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "RoundDirector");

    RoundDirector() : UnityEngine::MonoBehaviour(nullptr) {}
    RoundDirector(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    void ExtractionPointsUnlock()
    {
        THROW_IF_NULL();
        s_ExtractionPointsUnlock.CallThunk<void, RoundDirector>(*this);
    }

    STATIC_FIELD(instance, RoundDirector);
    FIELD(extractionPointActive, bool);
    FIELD(extractionPointCurrent, ExtractionPoint);
    FIELD(extractionPointList, System::List<UnityEngine::GameObject>);
    FIELD(cosmeticWorldObjects, System::List<CosmeticWorldObject>);

private:
    METHOD(ExtractionPointsUnlock);
};

struct ExtractionPoint_State
{
    META("Assembly-CSharp", "", "ExtractionPoint/State");

    ENUM_VALUE(Complete);
    ENUM_VALUE(Active);
    ENUM_VALUE(Idle);
};

struct TruckSafetySpawnPoint : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "TruckSafetySpawnPoint");

    TruckSafetySpawnPoint() : UnityEngine::MonoBehaviour(nullptr) {}
    TruckSafetySpawnPoint(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    STATIC_FIELD(instance, TruckSafetySpawnPoint);
};

struct PlayerDeathHead : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "PlayerDeathHead");

    PlayerDeathHead() : UnityEngine::MonoBehaviour(nullptr) {}
    PlayerDeathHead(UVM::Object* ptr) : UnityEngine::MonoBehaviour(ptr) {}

    inline operator bool() const { return !null(); }

    FIELD(colliders, System::Array<UnityEngine::Collider>);
    FIELD(meshRenderers, System::List<UnityEngine::MeshRenderer>);
    FIELD(eyeRenderers, System::Array<UnityEngine::MeshRenderer>);
};

struct MetaManager : UnityEngine::MonoBehaviour
{
    META("Assembly-CSharp", "", "MetaManager");

    inline operator bool() const { return !null(); }

    void CosmeticUnlockAll()
    {
        THROW_IF_NULL();
        s_CosmeticUnlockAll.CallThunk<bool, MetaManager>(*this);
    }

    STATIC_FIELD(instance, MetaManager);
private:
    METHOD(CosmeticUnlockAll);
};

static inline System::MethodInfo EventSystem_Update{{"UnityEngine.UI", "UnityEngine.EventSystems", "EventSystem"}, "Update"};
static inline System::MethodInfo Cursor_set_visible{{"UnityEngine.CoreModule", "UnityEngine", "Cursor"}, "set_visible"};
static inline System::MethodInfo PostProcessLayer_BuildCommandBuffers{{"Unity.Postprocessing.Runtime", "UnityEngine.Rendering.PostProcessing", "PostProcessLayer"}, "BuildCommandBuffers"};
static inline System::MethodInfo Physics_Raycast{{"UnityEngine.PhysicsModule", "UnityEngine", "Physics"}, "Raycast", "System.Boolean(UnityEngine.Vector3,UnityEngine.Vector3,UnityEngine.RaycastHit&,System.Single,System.Int32,UnityEngine.QueryTriggerInteraction)"};
static inline System::MethodInfo PhotonNetwork_IsMasterClient{{"PhotonUnityNetworking", "Photon.Pun", "PhotonNetwork"}, "get_IsMasterClient"};
static inline System::MethodInfo Camera_FireOnPreRender{{"UnityEngine.CoreModule", "UnityEngine", "Camera"}, "FireOnPreRender"};