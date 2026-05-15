#include "game_hooks.h"

#include "cheat.h"
#include "visuals.h"
#include "game_classes.h"

namespace Cheat::GameHooks
{
    static void Hooked__EventSystem_Update(UnityEngine::EventSystem __this);
    static void Hooked__PlayerHealth_Hurt(PlayerHealth __this, int damage, bool savingGrace, int enemyIndex);
    static void Hooked__PlayerAvatar_PlayerDeath(PlayerAvatar __this, int enemyIndex);
    static void Hooked__PlayerController_FixedUpdate(PlayerController __this);
    static void Hooked__PlayerController_Update(PlayerController __this);
    static void Hooked__PlayerTumble_TumbleRequest(PlayerTumble __this, bool isTumbling, bool playerInput);
    static void Hooked__FlashlightController_Update(FlashlightController __this);
    static void Hooked__ItemUpgrade_Update(ItemUpgrade __this);
    static void Hooked__ItemHealthPack_Update(ItemHealthPack __this);
    static void Hooked__UpdateLoop();
    static void Hooked__PhysGrabObject_PhysicsGrabbingManipulation(PhysGrabObject __this);
    static void Hooked__PostProcessLayer_BuildCommandBuffers(UnityEngine::PostProcessLayer __this);
    static void Hooked__PlayerAvatarVisuals_Update(PlayerAvatarVisuals __this);
    static void Hooked__PhysGrabber_PhysGrabLogic(PhysGrabber __this);
    static void Hooked__PhysGrabber_RayCheck(PhysGrabber __this, bool grab);
    static bool Hooked__Physics_Raycast(void* v1, void* v2, void* v3, float maxDistance, int v5, int v6);
    static void Hooked__PhysGrabObjectImpactDetector_Break(PhysGrabObjectImpactDetector __this, float a1, void* a2, int a3, bool a4);
    static void Hooked__PhysGrabObjectImpactDetector_DestroyObject(PhysGrabObjectImpactDetector __this, bool a1);
    static bool Hooked__PhotonNetwork_IsMasterClient();
    static void Hooked__ItemBattery_Update(ItemBattery __this);
    static void Hooked__Camera_FireOnPreRender(UnityEngine::Camera __this);
    static void Hooked__ItemGun_Update(ItemGun __this);
    static void Hooked__RunManager_SetRunLevel(RunManager __this);
    static void Hooked__PhysGrabber_PhysGrabOverCharge(PhysGrabber __this, float _amount, float _multiplier);
    static void Hooked__SpectateCamera_HeadEnergyLogic(SpectateCamera __this);
    static void Hooked__PostLateUpdateLoop();
    static void Hooked__PresentAfterDrawLoop();
    static void Hooked__EnemyRigidbody_FixedUpdate(EnemyRigidbody __this);

    static bool IsInGame();
    static void* GetPlayerLoopPtr(System::Type type, System::Type subType);
    static void Heal(PhysGrabObjectImpactDetector obj, float value);
    static void SpawnItem(Item item);
    static void ParseEnemies();
    static void ParseItems();
    static void ParseLevels();
    static void DrawPlayerChams(PlayerAvatar player, UnityEngine::CommandBuffer cb, UnityEngine::Material aliveMat, UnityEngine::Material deadMat);
    static Hax::Optional<Visuals::EnemyEspData> ParseEnemyEspData(Enemy enemy);
    static Hax::Optional<Visuals::ValuableEspData> ParseValuableEspData(ValuableObject obj);
    static Hax::Optional<Visuals::ExtrPointEspData> ParseExtrPointEspData(UnityEngine::GameObject obj);
    static Hax::Optional<Visuals::TruckEspData> ParseTruckEspData(TruckSafetySpawnPoint truck);
    static Hax::Optional<Visuals::PlayerEspData> ParsePlayerEspData(PlayerAvatar avatar);
    static UnityEngine::Rect CalcBoundsInScreenSpace(UnityEngine::Bounds bigBounds, UnityEngine::Camera cam);

    void Install()
    {
        Hax::LogDebug(GCheat->LogFile, L"Installing hooks...");

        #define HOOK(ptr, n) Cheat::Hook(ptr, Hooked__ ## n, GCheat-> ## n ## _Hook, #n)
        HOOK(EventSystem_Update.m_Pointer, EventSystem_Update);
        HOOK(PlayerHealth::s_Hurt.m_Pointer, PlayerHealth_Hurt);
        HOOK(PlayerAvatar::s_PlayerDeath.m_Pointer, PlayerAvatar_PlayerDeath);
        HOOK(PlayerController::s_FixedUpdate.m_Pointer, PlayerController_FixedUpdate);
        HOOK(PlayerController::s_Update.m_Pointer, PlayerController_Update);
        HOOK(PlayerTumble::s_TumbleRequest.m_Pointer, PlayerTumble_TumbleRequest);
        HOOK(FlashlightController::s_Update.m_Pointer, FlashlightController_Update);
        HOOK(ItemUpgrade::s_Update.m_Pointer, ItemUpgrade_Update);
        HOOK(ItemHealthPack::s_Update.m_Pointer, ItemHealthPack_Update);

        HOOK(PhysGrabObject::s_PhysicsGrabbingManipulation.m_Pointer, PhysGrabObject_PhysicsGrabbingManipulation);
        HOOK(PostProcessLayer_BuildCommandBuffers.m_Pointer, PostProcessLayer_BuildCommandBuffers);
        HOOK(PlayerAvatarVisuals::s_Update.m_Pointer, PlayerAvatarVisuals_Update);
        HOOK(PhysGrabber::s_PhysGrabLogic.m_Pointer, PhysGrabber_PhysGrabLogic);
        HOOK(PhysGrabber::s_RayCheck.m_Pointer, PhysGrabber_RayCheck);
        HOOK(Physics_Raycast.m_Pointer, Physics_Raycast);
        HOOK(PhysGrabObjectImpactDetector::s_Break.m_Pointer, PhysGrabObjectImpactDetector_Break);
        HOOK(PhysGrabObjectImpactDetector::s_DestroyObject.m_Pointer, PhysGrabObjectImpactDetector_DestroyObject);
        HOOK(PhotonNetwork_IsMasterClient.m_Pointer, PhotonNetwork_IsMasterClient);
        HOOK(ItemBattery::s_Update.m_Pointer, ItemBattery_Update);
        HOOK(Camera_FireOnPreRender.m_Pointer, Camera_FireOnPreRender);
        HOOK(ItemGun::s_Update.m_Pointer, ItemGun_Update);
        HOOK(RunManager::s_SetRunLevel.m_Pointer, RunManager_SetRunLevel);
        HOOK(PhysGrabber::s_PhysGrabOverCharge.m_Pointer, PhysGrabber_PhysGrabOverCharge);
        HOOK(SpectateCamera::s_HeadEnergyLogic.m_Pointer, SpectateCamera_HeadEnergyLogic);
        HOOK(EnemyRigidbody::s_FixedUpdate.m_Pointer, EnemyRigidbody_FixedUpdate);
        #undef HOOK
    }

    void LoopInstall()
    {
        HAX_PANIC(GCheat->LoopSystem.UpdatePtr != nullptr, &GCheat->LogFile, L"LoopSystem not ready");

        Cheat::Hook(GCheat->LoopSystem.UpdatePtr, Hooked__UpdateLoop, GCheat->LoopSystem.UpdateHook, "LoopSystem.Update");
        Cheat::Hook(GCheat->LoopSystem.PostLateUpdatePtr, Hooked__PostLateUpdateLoop, GCheat->LoopSystem.PostLateUpdateHook, "LoopSystem.PostLateUpdate");
        Cheat::Hook(GCheat->LoopSystem.PresentAfterDrawPtr, Hooked__PresentAfterDrawLoop, GCheat->LoopSystem.PresentAfterDrawHook, "LoopSystem.PresentAfterDrawLoop");
    }

    static void Hooked__EventSystem_Update(UnityEngine::EventSystem __this)
    {
        try
        {
            if (__this != UnityEngine::EventSystem::GetCurrent())
                return;

            PlayerAvatar avatar = PlayerAvatar::instance();
            PlayerHealth health = avatar ? avatar.playerHealth() : nullptr;

            GCheat->IsClient = !SemiFunc::IsMasterClientOrSingleplayer();
            GCheat->IsInGame = IsInGame();

            if (!GCheat->LoopSystem.Initialized)
            {
                GCheat->LoopSystem.UpdatePtr = GetPlayerLoopPtr(UnityEngine::PlayerLoop::Update::typeof(), UnityEngine::PlayerLoop::ScriptRunBehaviourUpdate::typeof());
                Hax::LogDebug(GCheat->LogFile, L"m_UpdatePtr ptr = %p", GCheat->LoopSystem.UpdatePtr);

                GCheat->LoopSystem.PostLateUpdatePtr = GetPlayerLoopPtr(UnityEngine::PlayerLoop::PostLateUpdate::typeof(), UnityEngine::PlayerLoop::PlayerSendFrameStarted::typeof());
                Hax::LogDebug(GCheat->LogFile, L"m_PostLateUpdatePtr ptr = %p", GCheat->LoopSystem.PostLateUpdatePtr);

                GCheat->LoopSystem.PresentAfterDrawPtr = GetPlayerLoopPtr(UnityEngine::PlayerLoop::PostLateUpdate::typeof(), UnityEngine::PlayerLoop::PresentAfterDraw::typeof());
                Hax::LogDebug(GCheat->LogFile, L"m_PresentAfterDrawPtr ptr = %p", GCheat->LoopSystem.PresentAfterDrawPtr);

                GCheat->LoopSystem.Initialized = true;
            }

            if (GCheat->HealToMax)
            {
                GCheat->HealToMax = false;
                if (health)
                    health.HealOther(99999, true);
            }

            if (GCheat->Upgrades.ItemToToggle)
            {
                ItemUpgrade item = GCheat->Upgrades.ItemToToggle;
                GCheat->Upgrades.ItemToToggle = nullptr;

                ItemToggle toggle = item.itemToggle();
                if (toggle)
                {
                    toggle.photonView() = item.photonView();
                    toggle.ToggleItem(true, SemiFunc::PhotonViewIDPlayerAvatarLocal());
                }
            }

            if (GCheat->Aids.ItemToToggle)
            {
                ItemHealthPack item = GCheat->Aids.ItemToToggle;
                GCheat->Aids.ItemToToggle = nullptr;

                ItemToggle toggle = item.itemToggle();
                if (toggle)
                {
                    toggle.photonView() = item.photonView();
                    toggle.ToggleItem(true, SemiFunc::PhotonViewIDPlayerAvatarLocal());
                }
            }

            if (GCheat->EnemiesPool.Empty())
                ParseEnemies();

            if (GCheat->ItemsPool.Empty())
                ParseItems();

            if (GCheat->LevelsBan.Empty())
                ParseLevels();

            if (GCheat->EnemyToSpawn)
            {
                EnemySetup enemy = GCheat->EnemyToSpawn;
                GCheat->EnemyToSpawn = nullptr;

                if (LevelGenerator gen = LevelGenerator::Instance())
                {
                    System::List<LevelPoint> points = gen.LevelPathPoints();
                    if (points && points.GetCount() > 0)
                    {
                        LevelPoint point = points[0];
                        if (point)
                            gen.EnemySpawn(enemy, point.GetTransform().GetPosition());
                    }
                }
            }

            /*if (EnemyDirector dir = EnemyDirector::instance())
            {
                dir.debugNoVision() = GCheat->Blind;
            }*/

            if (!GCheat->CommandBuffer)
            {
                GCheat->CommandBuffer = UnityEngine::CommandBuffer::New();
                UVM::GCHandleNew(*GCheat->CommandBuffer.GetPtr(), true);
            }

            if (!GCheat->Bundle)
            {
                auto ptr = Hax::Gui::GetResourceData((Hax::Handle)GCheat->hCheat, IDR_BUNDLE1, L"BUNDLE");

                System::Array<char> bundleRaw{UVM::ArrayNew(UVM::TypeGetClass(*System::Char::typeof().GetPtr()->Type), ptr.Size())};

                memcpy(bundleRaw.GetPtr()->begin(), ptr.Data(), ptr.Size());
                GCheat->Bundle = UnityEngine::AssetBundle::LoadFromMemory(bundleRaw);
                GCheat->Bundle.SetHideFlags(UnityEngine::HideFlags::DontUnloadUnusedAsset);

                Hax::LogDebug(GCheat->LogFile, L"Bundle loaded");
            }

            if (!GCheat->ItemsChamsMat && GCheat->Bundle)
            {
                UnityEngine::Shader shader{GCheat->Bundle.LoadAsset(System::String::New("assets/myshader.shader"), UnityEngine::Shader::typeof()).GetPtr()};

                GCheat->ItemsChamsMat = UnityEngine::Material::New(shader);
                GCheat->ItemsChamsMat.SetHideFlags(UnityEngine::HideFlags::DontUnloadUnusedAsset);
                GCheat->ItemsChamsMat.SetColor(UnityEngine::Color(1.f, 1.f, 0.9f, 1.f));
                UVM::GCHandleNew(*GCheat->ItemsChamsMat.GetPtr(), true);
                Hax::LogDebug(GCheat->LogFile, L"Items chams mat loaded");

                GCheat->PlayerChamsMat = UnityEngine::Material::New(shader);
                GCheat->PlayerChamsMat.SetHideFlags(UnityEngine::HideFlags::DontUnloadUnusedAsset);
                GCheat->PlayerChamsMat.SetColor(UnityEngine::Color::green());
                UVM::GCHandleNew(*GCheat->PlayerChamsMat.GetPtr(), true);
                Hax::LogDebug(GCheat->LogFile, L"Player chams mat loaded");

                GCheat->HeadChamsMat = UnityEngine::Material::New(shader);
                GCheat->HeadChamsMat.SetHideFlags(UnityEngine::HideFlags::DontUnloadUnusedAsset);
                GCheat->HeadChamsMat.SetColor(UnityEngine::Color(0x54 / 255.f, 0x54 / 255.f, 0x54 / 255.f, 1.f));
                UVM::GCHandleNew(*GCheat->HeadChamsMat.GetPtr(), true);
                Hax::LogDebug(GCheat->LogFile, L"Head chams mat loaded");
            }

            if (GCheat->SetToZero)
            {
                GCheat->SetToZero = false;
                if (ValuableDirector dir = ValuableDirector::instance())
                {
                    for (ValuableObject obj : dir.valuableList())
                    {
                        if (obj)
                            Heal(obj.physGrabObject().impactDetector(), -999999.f);
                    }
                }
            }

            if (GCheat->SetToMax)
            {
                GCheat->SetToMax = false;
                if (ValuableDirector dir = ValuableDirector::instance())
                {
                    for (ValuableObject obj : dir.valuableList())
                    {
                        if (obj)
                            Heal(obj.physGrabObject().impactDetector(), 999999.f);
                    }
                }
            }

            if (GCheat->ItemToSpawn)
            {
                Item item = GCheat->ItemToSpawn;
                GCheat->ItemToSpawn = nullptr;
                SpawnItem(item);
            }

            if (GCheat->ForceNextLevel)
            {
                GCheat->ForceNextLevel = false;
                if (RunManager manager = RunManager::instance())
                    manager.ChangeLevel(true, false);
            }

            if (GCheat->ActivateNextPoint)
            {
                GCheat->ActivateNextPoint = false;
                if (RoundDirector dir = RoundDirector::instance())
                {
                    for (UnityEngine::GameObject go : dir.extractionPointList())
                    {
                        ExtractionPoint point = go.GetComponent<ExtractionPoint>();
                        if (point && point.currentState() == ExtractionPoint_State::Idle() && !point.isLocked())
                        {
                            point.OnClick();
                            break;
                        }
                    }
                }
            }

            if (GCheat->TeleportToTruck)
            {
                GCheat->TeleportToTruck = false;
                if (avatar && !avatar.deadSet())
                    avatar.playerTransform().SetPosition(TruckSafetySpawnPoint::instance().GetTransform().GetPosition());
            }

            if (GCheat->ToTumble)
            {
                PlayerAvatar player = GCheat->ToTumble;
                GCheat->ToTumble = nullptr;
                if (player && !player.isDisabled())
                    player.tumble().TumbleRequest(true, false);
            }

            if (GCheat->ToKill)
            {
                PlayerAvatar player = GCheat->ToKill;
                GCheat->ToKill = nullptr;
                if (player && !player.deadSet() && !GCheat->IsClient)
                    player.PlayerDeath(-1);
            }

            if (GCheat->ToRevive)
            {
                PlayerAvatar player = GCheat->ToRevive;
                GCheat->ToRevive = nullptr;
                if (player && player.deadSet() && !GCheat->IsClient)
                    player.Revive(false);
            }

            PlayerVoiceChat chat = avatar ? avatar.voiceChat() : nullptr;
            if (chat && avatar.voiceChatFetched() && GCheat->PreferedChat != ChatPref::None)
            {
                bool isDeadPrefered = GCheat->PreferedChat == ChatPref::Dead;
                if (chat.inLobbyMixer() != isDeadPrefered)
                {
                    chat.ToggleMixer(isDeadPrefered, false);
                    Hax::LogDebug(GCheat->LogFile, L"Chat changed");
                }
                GCheat->PreferedChat = ChatPref::None;
            }

            if (GCheat->UnlockAllCosmetic && MetaManager::instance())
            {
                GCheat->UnlockAllCosmetic = false;
                MetaManager::instance().CosmeticUnlockAll();
            }

            /*for (size_t i = 0; i < GCheat->LoadRequests.size(); ++i)
            {
            LoadRequest* req = GCheat->LoadRequests[i];
            if (!req || !req->m_Request.GetIsDone())
            continue;

            GameObject go{req->m_Request.GetAsset().Backend()};
            if (go)
            {
            EnemyParent parent = go.GetComponent<EnemyParent>();
            if (parent)
            {
            std::string enemyName = parent.enemyName().UTF8();
            if (GCheat->EnemiesPool.emplace(enemyName, req->m_Setup).second)
            {
            HAX_LOG_DEBUG("Enemy parsed {}", enemyName);
            }
            }
            }

            delete req;
            GCheat->LoadRequests[i] = nullptr;
            }

            if (size_t errased = std::erase_if(GCheat->LoadRequests, [](auto& r) { return r == nullptr; }))
            {
            HAX_LOG_DEBUG("Erased {}. Left {}", errased, GCheat->LoadRequests.size());
            }*/
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->EventSystem_Update_Hook.unsafe_call<void, UnityEngine::EventSystem>(__this);
    }

    static void Hooked__PlayerHealth_Hurt(PlayerHealth __this, int damage, bool savingGrace, int enemyIndex)
    {
        if (GCheat->Godmode)
            return;

        GCheat->PlayerHealth_Hurt_Hook.unsafe_call<void, PlayerHealth, int, bool, int>(__this, damage, savingGrace, enemyIndex);
    }

    static void Hooked__PlayerAvatar_PlayerDeath(PlayerAvatar __this, int enemyIndex)
    {
        if (GCheat->Godmode && __this.isLocal())
            return;

        GCheat->PlayerAvatar_PlayerDeath_Hook.unsafe_call<void, PlayerAvatar, int>(__this, enemyIndex);
    }

    static void Hooked__PlayerController_FixedUpdate(PlayerController __this)
    {
        try
        {
            float& walkSpeed = __this.MoveSpeed();
            float& sprintSpeed = __this.SprintSpeed();
            float& crouchSpeed = __this.CrouchSpeed();

            float backup[3] = { walkSpeed, sprintSpeed, crouchSpeed };

            if (GCheat->InfStamina)
                __this.EnergyCurrent() = __this.EnergyStart();

            walkSpeed *= (float)GCheat->Acceleration.Walking;
            sprintSpeed *= (float)GCheat->Acceleration.Sprinting;
            crouchSpeed *= (float)GCheat->Acceleration.Crouching;

            GCheat->PlayerController_FixedUpdate_Hook.unsafe_call<void, PlayerController>(__this);

            walkSpeed = backup[0];
            sprintSpeed = backup[1];
            crouchSpeed = backup[2];
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");

            GCheat->PlayerController_FixedUpdate_Hook.unsafe_call<void, PlayerController>(__this);
        }
    }

    static void Hooked__PlayerController_Update(PlayerController __this)
    {
        int& jumps = __this.JumpExtra();
        int cachedJumps = jumps;
        if (GCheat->InfJumps)
            jumps = 9999;

        GCheat->PlayerController_Update_Hook.unsafe_call<void, PlayerController>(__this);

        jumps = cachedJumps;
    }

    static void Hooked__PlayerTumble_TumbleRequest(PlayerTumble __this, bool isTumbling, bool playerInput)
    {
        if (GCheat->NoTumble && __this.playerAvatar().isLocal() && !playerInput)
            return;

        GCheat->PlayerTumble_TumbleRequest_Hook.unsafe_call<void, PlayerTumble, bool, bool>(__this, isTumbling, playerInput);
    }

    static void Hooked__FlashlightController_Update(FlashlightController __this)
    {
        try
        {
            PlayerAvatar avatar = PlayerAvatar::instance();

            bool wasCrouching = false;
            if (GCheat->Flashlight.AllowInCrouch && avatar)
            {
                wasCrouching = avatar.isCrouching();
                avatar.isCrouching() = false;
            }

            GCheat->FlashlightController_Update_Hook.unsafe_call<void, FlashlightController>(__this);

            if (GCheat->Flashlight.AllowInCrouch && avatar)
                avatar.isCrouching() = wasCrouching;

            UnityEngine::Light spotLight = __this.spotlight();
            if ((float)GCheat->Flashlight.Angle != spotLight.GetSpotAngle())
            {
                spotLight.SetSpotAngle((float)GCheat->Flashlight.Angle);
            }

            spotLight.SetIntensity(spotLight.GetIntensity() * (GCheat->Flashlight.Intensity / 10.f));
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");

            GCheat->FlashlightController_Update_Hook.unsafe_call<void, FlashlightController>(__this);
        }
    }

    static void Hooked__ItemUpgrade_Update(ItemUpgrade __this)
    {
        try
        {
            GCheat->Upgrades.Pool.GetBack().PushBack(__this);

            if (GCheat->AutoUseUpgr && Visuals::IsUpgrAutouse(__this.itemAttributes().itemAssetName().GetPtr()))
            {
                ItemToggle toggle = __this.itemToggle();
                if (toggle)
                {
                    toggle.photonView() = __this.photonView();
                    toggle.ToggleItem(true, SemiFunc::PhotonViewIDPlayerAvatarLocal());
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->ItemUpgrade_Update_Hook.unsafe_call<void, ItemUpgrade>(__this);
    }

    static void Hooked__ItemHealthPack_Update(ItemHealthPack __this)
    {
        if (!RunManager::instance().levelIsShop())
            GCheat->Aids.Pool.GetBack().PushBack(__this);

        GCheat->ItemHealthPack_Update_Hook.unsafe_call<void, ItemHealthPack>(__this);
    }

    static void Hooked__UpdateLoop()
    {
        GCheat->Upgrades.Pool.GetBack().Clear();
        GCheat->Aids.Pool.GetBack().Clear();
        //GCheat->EnemiesEspBuffer.GetBack().clear();

        if (GCheat->LoopSystem.UpdateHook)
            GCheat->LoopSystem.UpdateHook.unsafe_call<void>();
        else
            Hax::LogError(GCheat->LogFile, L"Loop not ready");

        GCheat->Upgrades.Pool.RefreshSpare();
        GCheat->Aids.Pool.RefreshSpare();
        //GCheat->EnemiesEspBuffer.RefreshSpare();
    }

    static void SetThirdPerson()
    {
        static System::String playerTag;
        if (!playerTag)
        {
            playerTag = System::String::New("Player");
            UVM::GCHandleNew(*playerTag.GetPtr(), true);
        }

        PlayerAvatar avatar = PlayerAvatar::instance();

        float dist = 3.f;
        UnityEngine::Transform normalTransformPivot = avatar.spectatePoint();
        static int layerMask = SemiFunc::LayerMaskGetVisionObstruct();
        auto hits = UnityEngine::Physics::SphereCastAll(normalTransformPivot.GetPosition(), 0.1f, normalTransformPivot.GetForward() * -1.f, 3.f, layerMask);
        bool isMapOpen = avatar.mapToolController() && avatar.mapToolController().Active();
        if (hits && !isMapOpen)
        {
            for (UnityEngine::RaycastHit& hit : hits)
            {
                UnityEngine::Transform transform = hit.GetTransform();
                if (!transform.GetComponent<PlayerHealthGrab>() && !transform.GetGameObject().CompareTag(playerTag) && !transform.GetComponent<PlayerTumble>() && !transform.GetComponent<EnemyRigidbody>())
                    dist = Hax::Min(Hax::Max(1.f, hit.m_Distance), 3.f);
            }
        }
        else if (isMapOpen)
            dist = 1.f;

        SemiFunc::MainCamera().GetTransform().SetLocalPosition(UnityEngine::Vector3(0.f, 0.f, -dist));
    }

    static bool ShouldDrawEsp()
    {
        return GCheat->IsInGame && MenuManager::instance() && !MenuManager::instance().currentMenuPage();
    }

    static float prevPlane;
    static float prevFov;
    static void Hooked__PostLateUpdateLoop()
    {
        try
        {
            UnityEngine::Camera mainCam = SemiFunc::MainCamera();
            PlayerAvatar avatar = PlayerAvatar::instance();
            if (mainCam && GCheat->IsInGame && avatar)
            {
                if (GCheat->ImproveVision)
                {
                    prevPlane = mainCam.GetFarClipPlane();
                    if (prevPlane < 32.f)
                        mainCam.SetFarClipPlane(32.f);
                }

                prevFov = mainCam.GetFieldOfView();
                if (prevFov < (float)GCheat->FOV)
                    mainCam.SetFieldOfView((float)GCheat->FOV);

                if (GCheat->ThirdPerson && !SpectateCamera::instance())
                    SetThirdPerson();

                if (ShouldDrawEsp())
                {
                    GCheat->PixelHeight = (float)mainCam.GetPixelHeight();
                    GCheat->PixelWidth = (float)mainCam.GetPixelWidth();
                    GCheat->ScreenHeight = (float)UnityEngine::Screen::GetHeight();
                    GCheat->ScreenWidth = (float)UnityEngine::Screen::GetWidth();

                    if (GCheat->EnemiesEsp)
                    {
                        auto& back = GCheat->EnemiesEspBuffer.GetBack();
                        back.Clear();
                        if (EnemyDirector dir = EnemyDirector::instance())
                        {
                            for (EnemyParent parent : dir.enemiesSpawned())
                            {
                                Enemy enemy = parent.enemy();
                                if (enemy && parent.Spawned())
                                {
                                    Hax::Optional<Visuals::EnemyEspData> data = ParseEnemyEspData(enemy);
                                    if (data.HasValue())
                                        back.PushBack(*data);
                                }
                            }
                        }
                        GCheat->EnemiesEspBuffer.RefreshSpare();
                    }

                    if (GCheat->ValuablesEsp)
                    {
                        auto& back = GCheat->ValuablesEspBuffer.GetBack();
                        back.Clear();
                        if (ValuableDirector dir = ValuableDirector::instance())
                        {
                            for (ValuableObject obj : dir.valuableList())
                            {
                                Hax::Optional<Visuals::ValuableEspData> data = ParseValuableEspData(obj);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        GCheat->ValuablesEspBuffer.RefreshSpare();
                    }

                    if (GCheat->ExtrPointsEsp)
                    {
                        auto& back = GCheat->ExtrPointsEspBuffer.GetBack();
                        back.Clear();
                        if (RoundDirector dir = RoundDirector::instance())
                        {
                            for (UnityEngine::GameObject go : dir.extractionPointList())
                            {
                                Hax::Optional<Visuals::ExtrPointEspData> data = ParseExtrPointEspData(go);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        GCheat->ExtrPointsEspBuffer.RefreshSpare();
                    }

                    if (GCheat->TruckEsp)
                    {
                        auto& back = GCheat->TruckEspBuffer.GetBack();
                        back.Clear();
                        if (TruckSafetySpawnPoint truck = TruckSafetySpawnPoint::instance())
                        {
                            Hax::Optional<Visuals::TruckEspData> data = ParseTruckEspData(truck);
                            if (data.HasValue())
                                back.PushBack(*data);
                        }
                        GCheat->TruckEspBuffer.RefreshSpare();
                    }

                    if (GCheat->PlayersEsp)
                    {
                        auto& back = GCheat->PlayersEspBuffer.GetBack();
                        back.Clear();
                        if (GameDirector dir = GameDirector::instance())
                        {
                            for (PlayerAvatar avatar : dir.PlayerList())
                            {
                                Hax::Optional<Visuals::PlayerEspData> data = ParsePlayerEspData(avatar);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        GCheat->PlayersEspBuffer.RefreshSpare();
                    }
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->LoopSystem.PostLateUpdateHook.unsafe_call<void>();
    }

    static void Hooked__PresentAfterDrawLoop()
    {
        try
        {
            UnityEngine::Camera mainCam = SemiFunc::MainCamera();
            if (mainCam && GCheat->IsInGame)
            {
                if (GCheat->ImproveVision)
                    mainCam.SetFarClipPlane(prevPlane);

                mainCam.SetFieldOfView(prevFov);

                if (GCheat->ThirdPerson && !SpectateCamera::instance())
                    mainCam.GetTransform().SetLocalPosition(UnityEngine::Vector3::zero());
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->LoopSystem.PresentAfterDrawHook.unsafe_call<void>();
    }

    static void Hooked__PhysGrabObject_PhysicsGrabbingManipulation(PhysGrabObject __this)
    {
        try
        {
            if (PlayerAvatar avatar = PlayerAvatar::instance())
            {
                if (PhysGrabber grabber = avatar.physGrabber())
                {
                    float backup = grabber.grabStrength();
                    if (GCheat->EasyGrab && backup < 20.f)
                        grabber.grabStrength() = 20.f;

                    GCheat->PhysGrabObject_PhysicsGrabbingManipulation_Hook.unsafe_call<void, PhysGrabObject>(__this);

                    grabber.grabStrength() = backup;
                    return;
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->PhysGrabObject_PhysicsGrabbingManipulation_Hook.unsafe_call<void, PhysGrabObject>(__this);
    }

    static void Hooked__PostProcessLayer_BuildCommandBuffers(UnityEngine::PostProcessLayer __this)
    {
        try
        {
            if (GCheat->ImproveVision && GCheat->IsInGame)
            {
                __this.m_LegacyCmdBufferBeforeReflections().Clear();
                __this.m_LegacyCmdBufferBeforeLighting().Clear();
                __this.m_LegacyCmdBufferOpaque().Clear();
                __this.m_LegacyCmdBuffer().Clear();
                return;
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->PostProcessLayer_BuildCommandBuffers_Hook.unsafe_call<void, UnityEngine::PostProcessLayer>(__this);
    }

    void Hooked__PlayerAvatarVisuals_Update(PlayerAvatarVisuals __this)
    {
        try
        {
            if (GameManager gm = GameManager::instance())
            {
                PlayerAvatar avatar = __this.playerAvatar();
                if (GCheat->ThirdPerson && avatar && !__this.isMenuAvatar() && avatar.isLocal())
                {
                    __this.showSelfOverrideTimer() = Hax::Max(0.1f, __this.showSelfOverrideTimer());

                    /*UnityEngine::GameObject go = __this.meshParent();
                    if (go.GetActiveSelf() != GCheat->ThirdPerson)
                    {
                        go.SetActive(GCheat->ThirdPerson);
                    }

                    UnityEngine::Behaviour anim = __this.animator();
                    if (anim.GetEnabled() != GCheat->ThirdPerson)
                    {
                        anim.SetEnabled(GCheat->ThirdPerson);
                    }

                    int& mode = gm.gameMode();
                    int prevMode = mode;
                    if (GCheat->ThirdPerson)
                    {
                        mode = 1;
                        avatar.photonView().IsMine() = false;
                    }*/

                    //GCheat->PlayerAvatarVisuals_Update_Hook.unsafe_call<void, PlayerAvatarVisuals>(__this);

                    /*if (GCheat->ThirdPerson)
                    {
                        __this.GetTransform().SetPosition(avatar.GetTransform().GetPosition());
                        __this.GetTransform().SetRotation(avatar.GetTransform().GetRotation());
                        avatar.photonView().IsMine() = true;
                        mode = prevMode;
                    }*/
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->PlayerAvatarVisuals_Update_Hook.unsafe_call<void, PlayerAvatarVisuals>(__this);
    }

    static void Hooked__PhysGrabber_PhysGrabLogic(PhysGrabber __this)
    {
        float& range = __this.grabRange();
        float cached = range;
        PlayerAvatar playerAvatar = PlayerAvatar::instance();

        if (GCheat->UnlimGrabRange && playerAvatar && __this == playerAvatar.physGrabber())
            range = 999999.f;

        GCheat->PhysGrabber_PhysGrabLogic_Hook.unsafe_call<void, PhysGrabber>(__this);

        range = cached;
    }

    static bool g_RaycastMaxDist;
    static void Hooked__PhysGrabber_RayCheck(PhysGrabber __this, bool grab)
    {
        float& range = __this.grabRange();
        float cached = range;
        PlayerAvatar playerAvatar = PlayerAvatar::instance();

        if (GCheat->UnlimGrabRange && playerAvatar && __this == playerAvatar.physGrabber()) 
        {
            range = 999999.f;
            g_RaycastMaxDist = true;
        }

        GCheat->PhysGrabber_RayCheck_Hook.unsafe_call<void, PhysGrabber, bool>(__this, grab);

        range = cached;
        g_RaycastMaxDist = false;
    }

    static bool Hooked__Physics_Raycast(void* v1, void* v2, void* v3, float maxDistance, int v5, int v6)
    {
        if (g_RaycastMaxDist)
            maxDistance = 999999.f;

        return GCheat->Physics_Raycast_Hook.unsafe_call<bool, void*, void*, void*, float, int, int>(v1, v2, v3, maxDistance, v5, v6);
    }

    static void Hooked__PhysGrabObjectImpactDetector_Break(PhysGrabObjectImpactDetector __this, float a1, void* a2, int a3, bool a4)
    {
        if (GCheat->Unbreakable && __this.isValuable())
            return;

        GCheat->PhysGrabObjectImpactDetector_Break_Hook.unsafe_call<void, PhysGrabObjectImpactDetector, float, void*, int, bool>(__this, a1, a2, a3, a4);
    }

    static void Hooked__PhysGrabObjectImpactDetector_DestroyObject(PhysGrabObjectImpactDetector __this, bool a1)
    {
        if (GCheat->Unbreakable && __this.isValuable())
            return;

        GCheat->PhysGrabObjectImpactDetector_DestroyObject_Hook.unsafe_call<void, PhysGrabObjectImpactDetector, bool>(__this, a1);
    }

    static bool g_PretendMaster;
    static bool Hooked__PhotonNetwork_IsMasterClient()
    {
        if (g_PretendMaster)
        {
            g_PretendMaster = false;
            return true;
        }
        return GCheat->PhotonNetwork_IsMasterClient_Hook.unsafe_call<bool>();
    }

    static void Hooked__ItemBattery_Update(ItemBattery __this)
    {
        try
        {
            if (GCheat->InfBattery && __this.batteryLifeInt() < __this.batteryBars() && __this.physGrabObject().grabbedLocal())
            {
                //g_PretendMaster = true;
                __this.BatteryFullPercentChange(__this.batteryBars(), false);
                //g_PretendMaster = false;
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->ItemBattery_Update_Hook.unsafe_call<void, ItemBattery>(__this);
    }

    static void Hooked__Camera_FireOnPreRender(UnityEngine::Camera __this)
    {
        try
        {
            if (IsInGame() && __this == GameDirector::instance().MainCamera() && GCheat->ItemsChamsMat && GCheat->CommandBuffer)
            {
                GCheat->CommandBuffer.Clear();

                static void* s_PrevCam;
                if (__this && __this.GetPtr()->m_CachedPtr != s_PrevCam)
                {
                    s_PrevCam = __this.GetPtr()->m_CachedPtr;
                    __this.AddCommandBuffer(UnityEngine::CameraEvent::AfterEverything, GCheat->CommandBuffer);
                }

                if (GCheat->ItemsChams && ValuableDirector::instance())
                {
                    for (ValuableObject obj : ValuableDirector::instance().valuableList())
                    {
                        if (obj && obj.GetEnabled())
                        {
                            for (UnityEngine::MeshRenderer renderer : obj.GetTransform().GetChild(0).GetComponentsInChildren<UnityEngine::MeshRenderer>(true))
                            {
                                GCheat->CommandBuffer.DrawRenderer(renderer, GCheat->ItemsChamsMat);
                            }
                        }
                    }
                }

                if (GCheat->PlayerChams && GameDirector::instance())
                {
                    for (PlayerAvatar player : GameDirector::instance().PlayerList())
                    {
                        DrawPlayerChams(player, GCheat->CommandBuffer, GCheat->PlayerChamsMat, GCheat->HeadChamsMat);
                    }
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->Camera_FireOnPreRender_Hook.unsafe_call<void, UnityEngine::Camera>(__this);
    }

    static void Hooked__ItemGun_Update(ItemGun __this)
    {
        try
        {
            UnityEngine::LineRenderer laser = __this.GetComponent<UnityEngine::LineRenderer>();

            if (__this.physGrabObject().grabbedLocal() && GCheat->UseLaser)
            {
                if (!laser)
                {
                    laser = __this.GetGameObject().AddComponent<UnityEngine::LineRenderer>();
                    laser.SetStartWidth(0.02f);
                    laser.SetEndWidth(0.02f);
                    laser.SetSortingOrder(1);
                    UnityEngine::Material mat = UnityEngine::Material::New(UnityEngine::Shader::Find(System::String::New("Sprites/Default")));
                    mat.SetColor(UnityEngine::Color::red());
                    laser.SetMaterial(mat);
                    laser.SetPositionCount(2);
                }

                UnityEngine::Transform transform = __this.gunMuzzle();
                laser.SetEnabled(true);
                laser.SetPosition(0, transform.GetPosition());
                laser.SetPosition(1, transform.GetPosition() + transform.GetForward() * __this.gunRange());
            }
            else if (laser)
            {
                laser.SetEnabled(false);
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->ItemGun_Update_Hook.unsafe_call<void, ItemGun>(__this);
    }

    static void Hooked__RunManager_SetRunLevel(RunManager __this)
    {
        try
        {
            if (GCheat->LevelsBan.Size() > 0)
            {
                System::List<Level> levels = RunManager::instance().levels();

                LevelBan* levelBan = &GCheat->LevelsBan[0];
                Level prevLevel = __this.previousRunLevel();

                while (!levelBan->Allowed || ((GCheat->LevelsBan.Size() - GCheat->TotalBans) > 1 && prevLevel && levels[levelBan->Index] == prevLevel))
                    levelBan = &GCheat->LevelsBan[rand() % GCheat->LevelsBan.Size()];

                __this.levelCurrent() = levels[levelBan->Index];
                return;
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        GCheat->RunManager_SetRunLevel_Hook.unsafe_call<void, RunManager>(__this);
    }

    static void Hooked__PhysGrabber_PhysGrabOverCharge(PhysGrabber __this, float _amount, float _multiplier)
    {
        if (GCheat->NoOverCharge)
            return;

        GCheat->PhysGrabber_PhysGrabOverCharge_Hook.unsafe_call<void, PhysGrabber, float, float>(__this, _amount, _multiplier);
    }

    static void Hooked__SpectateCamera_HeadEnergyLogic(SpectateCamera __this)
    {
        if (GCheat->HeadMaxBattery)
        {
            __this.headEnergy() = 9999.f;
            __this.headEnergyEnough() = true;
        }

        GCheat->SpectateCamera_HeadEnergyLogic_Hook.unsafe_call<void, SpectateCamera>(__this);
    }

    static void Hooked__EnemyRigidbody_FixedUpdate(EnemyRigidbody __this)
    {
        if (GCheat->NoGrabMaxTime && SemiFunc::IsMasterClientOrSingleplayer())
        {
            __this.grabShakeReleaseTimer() = 0.f;
            __this.grabTimeCurrent() = 0.f;
        }

        GCheat->EnemyRigidbody_FixedUpdate_Hook.unsafe_call<void, EnemyRigidbody>(__this);
    }

    static bool IsInGame()
    {
        RunManager runManager = RunManager::instance();
        GameDirector gameDirector = GameDirector::instance();
        return runManager && PlayerAvatar::instance() && gameDirector
            && runManager.levelCurrent() != runManager.levelLobbyMenu()
            && runManager.levelCurrent() != runManager.levelMainMenu()
            && gameDirector.currentState() == GameDirector_gameState::Main();
    }

    static void* GetPlayerLoopPtr(System::Type type, System::Type subType)
    {
        auto mainLoop = UnityEngine::LowLevel::PlayerLoop::GetDefaultPlayerLoop();
        for (UnityEngine::PlayerLoopSystem& loop : mainLoop.m_SubSystemList)
        {
            if (loop.m_Type != type)
                continue;

            for (UnityEngine::PlayerLoopSystem& subLoop : loop.m_SubSystemList)
                if (subLoop.m_Type == subType)
                    return *subLoop.m_UpdateFunction;
        }
        return nullptr;
    }

    static void Heal(PhysGrabObjectImpactDetector obj, float value)
    {
        if (SemiFunc::IsMultiplayer())
        {
            System::Array<System::Object> arr = UVM::ArrayNew(UVM::TypeGetClass(*System::Object::typeof().GetPtr()->Type), 2);
            System::Boxed<System::Single> p1 = System::Single(value); arr[0] = System::Object(&p1);
            System::Boxed<UnityEngine::Vector3> p2{}; arr[1] = System::Object(&p2);
            obj.photonView().RPC(System::String::New("HealRPC"), Photon::RpcTarget::All,  arr);
        }
        else
        {
            obj.HealLogic(value, UnityEngine::Vector3::zero());
        }
    }

    static void SpawnItem(Item item)
    {
        UnityEngine::Camera camera = SemiFunc::MainCamera();
        if (!camera)
            return;

        if (!item)
            return;

        if (SemiFunc::IsMasterClient())
        {
            UnityEngine::Transform transform = camera.GetTransform();
            UnityEngine::Vector3 pos = transform.GetPosition() + transform.GetForward() - transform.GetUp();
            Photon::PhotonNetwork::InstantiateRoomObject(item.prefab().resourcePath(), pos, UnityEngine::Quaternion::identity());
        }
        if (!SemiFunc::IsMultiplayer())
        {
            UnityEngine::Transform transform = camera.GetTransform();
            UnityEngine::Vector3 pos = transform.GetPosition() + transform.GetForward() * 2.f - transform.GetUp();
            UnityEngine::Object::Instantiate<UnityEngine::GameObject>(item.prefab().Prefab(), pos, UnityEngine::Quaternion::identity());
        }
    }

    static void ParseEnemies()
    {
        EnemyDirector dir = EnemyDirector::instance();
        if (!GCheat->EnemiesPool.Empty() || !dir)
            return;

        Hax::LogDebug(GCheat->LogFile, L"Parsing enemies");
        //GCheat->LoadRequests.reserve(50);

        System::List<EnemySetup> setups[3] = {dir.enemiesDifficulty1(), dir.enemiesDifficulty2(), dir.enemiesDifficulty3()};
        for (size_t i = 0; i < 3; ++i)
        {
            for (EnemySetup setup : setups[i])
            {
                if (!setup)
                    continue;

                System::String name = setup.GetName();
                if (name.Contains(L"Bang") || name.Contains(L"Gnome"))
                    continue;

                for (PrefabRef ref : setup.spawnObjects())
                {
                    auto req = UnityEngine::Resources::LoadAsync(ref.resourcePath());
                    //GCheat->LoadRequests.push_back(new LoadRequest(setup, req));

                    Hax::WStringView enemyName = ref.prefabName().begin() + 8;

                    EnemySetup& entry = GCheat->EnemiesPool.FindOrAdd(enemyName);
                    if (!entry)
                    {
                        entry = setup;
                        UVM::GCHandleNew(*name.GetPtr(), true);
                        Hax::LogDebug(GCheat->LogFile, L"Enemy parsed %ls", enemyName.begin());
                    }
                }
            }
        }
    }

    static void ParseItems()
    {
        if (StatsManager manager = StatsManager::instance())
        {
            auto itemDict = manager.itemDictionary();
            for (int i = 0; i < itemDict.Count(); ++i)
            {
                auto entry = itemDict.begin() + i;
                System::String itemName = entry->Key;

                Hax::char16* ptr = itemName.begin();
                if (wcsncmp(ptr, L"Item ", 5) == 0)
                    ptr += 5;

                Hax::WStringView name = ptr;

                if (!GCheat->ItemsPool.Contains(name))
                {
                    if (entry->Value)
                    {
                        GCheat->ItemsPool.Insert(name, i);
                        auto req = UnityEngine::Resources::LoadAsync(entry->Value.prefab().resourcePath());

                        UVM::GCHandleNew(*itemName.GetPtr(), true);
                        Hax::LogDebug(GCheat->LogFile, L"Item parsed %ls", ptr);
                    }
                    else
                        Hax::LogError(GCheat->LogFile, L"Unable to parse item %s", ptr);
                }
            }
        }
    }

    static void ParseLevels()
    {
        if (RunManager manager = RunManager::instance())
        {
            Hax::LogDebug(GCheat->LogFile, L"Parsing levels");

            System::List<Level> levels = manager.levels();
            GCheat->LevelsBan.Reserve(levels.GetCount());

            for (int i = 0; i < levels.GetCount(); ++i)
            {
                System::String levelName = levels[i].NarrativeName();

                LevelBan ban = {levelName.ToView(), i, true};
                GCheat->LevelsBan.PushBack(ban);

                UVM::GCHandleNew(*levelName.GetPtr(), true);
                Hax::LogDebug(GCheat->LogFile, L"Parsed level %ls", levelName.begin());
            }

            std::sort(GCheat->LevelsBan.begin(), GCheat->LevelsBan.end(), [](const LevelBan& o1, const LevelBan& o2) { return o1.Name < o2.Name; });
        }
    }

    static void DrawPlayerChams(PlayerAvatar player, UnityEngine::CommandBuffer cb, UnityEngine::Material aliveMat, UnityEngine::Material deadMat)
    {
        if (!cb || !aliveMat || !deadMat || !player || player.isLocal())
            return;

        if (player.deadSet())
        {
            PlayerDeathHead head = player.playerDeathHead();
            if (head && head.GetEnabled())
            {
                for (UnityEngine::MeshRenderer r : head.meshRenderers())
                    cb.DrawRenderer(r, deadMat);
            }
            return;
        }

        if (!player.isDisabled())
        {
            for (UnityEngine::MeshRenderer renderer : player.playerHealth().renderers())
            {
                if (renderer && renderer.GetEnabled())
                    cb.DrawRenderer(renderer, aliveMat);
            }
        }
    }

    static Hax::Optional<Visuals::EnemyEspData> ParseEnemyEspData(Enemy enemy)
    {
        UnityEngine::Camera mainCam = GameDirector::instance().MainCamera();
        if (!mainCam || !enemy)
            return {};

        float dist = mainCam.GetTransform().GetPosition().Distance(enemy.GetTransform().GetPosition());
        if (dist < 2.f)
            return {};

        EnemyParent parent = enemy.enemyParent();
        if (parent.enemyName() == L"Spewer")
        {
            EnemySlowMouth comp = enemy.GetComponent<EnemySlowMouth>();
            if (comp && comp.attachedTimer() > 0.f)
                return {};
        }

        UnityEngine::Bounds enemyBounds{enemy.CenterTransform().GetPosition(), UnityEngine::Vector3()};
        if (EnemyRigidbody rb = enemy.Rigidbody())
        {
            PhysGrabObject grabObj = rb.physGrabObject();
            enemyBounds = UnityEngine::Bounds(grabObj.centerPoint(), grabObj.boundingBox());
        }
        else
        {
            enemyBounds = enemy.GetTransform().GetChild(1).GetChild(0).GetComponent<UnityEngine::Collider>().GetBounds();
        }

        UnityEngine::Rect bounds = CalcBoundsInScreenSpace(enemyBounds, mainCam);
        UnityEngine::Rect screenRect{0.f, 0.f, GCheat->ScreenWidth, GCheat->ScreenHeight};

        if (!bounds.Overlaps(screenRect))
            return {};

        EnemyHealth health = enemy.Health();

        Visuals::EnemyEspData espData
        {
            .Box = bounds.ToHax(),
            .Name = enemy.enemyParent().enemyName().ToView(),
            .Distance = dist,
            .CurHp = health.healthCurrent(),
            .MaxHp = health.health(),
        };
        return espData;
    }

    static Hax::Optional<Visuals::ValuableEspData> ParseValuableEspData(ValuableObject obj)
    {
        if (!obj || !obj.GetEnabled())
            return {};

        UnityEngine::Camera cam = GameDirector::instance().MainCamera();
        float dist = cam.GetTransform().GetPosition().Distance(obj.GetTransform().GetPosition());
        if (dist < 2.f || dist > (float)GCheat->ValuablesEspDistance)
            return {};

        PhysGrabObject physObject = obj.physGrabObject();
        if (!physObject)
            return {};

        UnityEngine::Vector3 worldPos = physObject.midPoint();
        UnityEngine::Vector3 screenPos = cam.WorldToScreenPoint(worldPos);
        UnityEngine::Rect screenRect{0.f, 0.f, GCheat->ScreenWidth, GCheat->ScreenHeight};
        if (screenPos.z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = GCheat->ScreenWidth / GCheat->PixelWidth;
        float scaleY = GCheat->ScreenHeight / GCheat->PixelHeight;

        screenPos.x *= scaleX;
        screenPos.y = GCheat->ScreenHeight - screenPos.y * scaleY;

        Visuals::ValuableEspData data
        {
            .Name = obj.GetName().ToView(),
            .Pos = screenPos.ToVector2().ToHax(),
            .Distance = dist,
            .Value = obj.dollarValueCurrent()
        };
        return data;
    }

    static Hax::Optional<Visuals::ExtrPointEspData> ParseExtrPointEspData(UnityEngine::GameObject obj)
    {
        UnityEngine::Camera cam = GameDirector::instance().MainCamera();

        if (!obj || !cam)
            return {};

        ExtractionPoint point = obj.GetComponent<ExtractionPoint>();
        if (!point)
            return {};

        float dist = cam.GetTransform().GetPosition().Distance(obj.GetTransform().GetPosition());
        if (dist < 1.f)
            return {};

        UnityEngine::Vector3 screenPos = cam.WorldToScreenPoint(obj.GetTransform().GetPosition());
        UnityEngine::Rect screenRect{0.f, 0.f, GCheat->ScreenWidth, GCheat->ScreenHeight};
        if (screenPos.z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = GCheat->ScreenWidth / GCheat->PixelWidth;
        float scaleY = GCheat->ScreenHeight / GCheat->PixelHeight;

        screenPos.x *= scaleX;
        screenPos.y = GCheat->ScreenHeight - screenPos.y * scaleY;

        Visuals::ExtrPointEspData data{};
        data.Completed = point.currentState() == ExtractionPoint_State::Complete();
        data.Active = point == RoundDirector::instance().extractionPointCurrent();
        data.Pos = screenPos.ToVector2().ToHax();
        data.Distance = dist;

        return data;
    }

    static Hax::Optional<Visuals::TruckEspData> ParseTruckEspData(TruckSafetySpawnPoint truck)
    {
        UnityEngine::Camera cam = GameDirector::instance().MainCamera();

        if (!truck || !cam)
            return {};

        UnityEngine::Vector3 worldPos = truck.GetTransform().GetPosition();

        float dist = cam.GetTransform().GetPosition().Distance(worldPos);
        if (dist < 1.f)
            return {};

        UnityEngine::Vector3 screenPos = cam.WorldToScreenPoint(worldPos);
        UnityEngine::Rect screenRect{0.f, 0.f, GCheat->ScreenWidth, GCheat->ScreenHeight};
        if (screenPos.z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = GCheat->ScreenWidth / GCheat->PixelWidth;
        float scaleY = GCheat->ScreenHeight / GCheat->PixelHeight;

        screenPos.x *= scaleX;
        screenPos.y = GCheat->ScreenHeight - screenPos.y * scaleY;

        Visuals::TruckEspData data{};
        data.Pos = screenPos.ToVector2().ToHax();
        data.Distance = dist;

        return data;
    }

    static Hax::Optional<Visuals::PlayerEspData> ParsePlayerEspData(PlayerAvatar avatar)
    {
        UnityEngine::Camera mainCam = GameDirector::instance().MainCamera();
        if (!mainCam || !avatar || avatar.isLocal())
            return {};

        float dist = mainCam.GetTransform().GetPosition().Distance(avatar.GetTransform().GetPosition());
        if (dist < 1.f)
            return {};

        if (avatar.deadSet())
        {
            PlayerDeathHead head = avatar.playerDeathHead();
            if (head && head.GetEnabled())
            {
                System::Array<UnityEngine::Collider> colliders = head.colliders();
                UnityEngine::Bounds bounds = colliders[0].GetBounds();
                for (auto col : colliders)
                {
                    bounds.Encapsulate(col.GetBounds());
                }

                if (bounds.m_Extents == UnityEngine::Vector3::zero())
                    return {};

                UnityEngine::Rect rect = CalcBoundsInScreenSpace(bounds, mainCam);
                UnityEngine::Rect screenRect{0.f, 0.f, GCheat->ScreenWidth, GCheat->ScreenHeight};

                if (!rect.Overlaps(screenRect))
                    return {};

                Visuals::PlayerEspData espData{};
                espData.Box = rect.ToHax();
                espData.Distance = dist;
                espData.Name = SemiFunc::PlayerGetName(avatar).ToView();
                espData.CurHp = 0;
                espData.MaxHp = 0;
                espData.Dead = true;

                return espData;
            }

            return {};
        }

        if (avatar.isDisabled())
            return {};

        PlayerAvatarVisuals visuals = avatar.playerAvatarVisuals();
        UnityEngine::Bounds bounds2 = visuals.headSideTransform().GetComponent<UnityEngine::Collider>().GetBounds();
        UnityEngine::Vector3 ext = bounds2.m_Extents;
        for (UnityEngine::Collider col2 : visuals.legTwistTransform().GetComponentsInChildren<UnityEngine::Collider>())
        {
            if (col2)
            {
                bounds2.Encapsulate(col2.GetBounds());
            }
        }
        if (bounds2.m_Extents == UnityEngine::Vector3::zero())
        {
            return {};
        }

        ext.y = bounds2.m_Extents.y * 1.2f;
        ext.z *= 1.5f;
        ext.x *= 1.5f;
        bounds2.m_Extents = ext;
        UnityEngine::Rect rect = CalcBoundsInScreenSpace(bounds2, mainCam);

        UnityEngine::Rect screenRect{0.f, 0.f, GCheat->ScreenWidth, GCheat->ScreenHeight};
        if (!rect.Overlaps(screenRect))
            return {};

        Visuals::PlayerEspData espData{};
        espData.Box = rect.ToHax();
        espData.Distance = dist;
        espData.Name = SemiFunc::PlayerGetName(avatar).ToView();
        espData.Dead = false;

        PlayerHealth health = avatar.playerHealth();
        espData.CurHp = health.health();
        espData.MaxHp = health.maxHealth();

        return espData;
    }

    static UnityEngine::Rect CalcBoundsInScreenSpace(UnityEngine::Bounds bigBounds, UnityEngine::Camera cam)
    {
        float scaleX = GCheat->ScreenWidth / GCheat->PixelWidth;
        float scaleY = GCheat->ScreenHeight / GCheat->PixelHeight;

        UnityEngine::Vector3 screenSpaceCorners[8] =
        {
            {bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z},
            {bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z},
            {bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z},
            {bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z},
            {bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z},
            {bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z},
            {bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z},
            {bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z}
        };

        for (int i = 0; i < 8; ++i)
        {
            screenSpaceCorners[i] = cam.WorldToScreenPoint(screenSpaceCorners[i]);
            screenSpaceCorners[i].x *= scaleX;
            screenSpaceCorners[i].y = GCheat->ScreenHeight - screenSpaceCorners[i].y * scaleY;
        }

        float x = FLT_MAX;
        float y = FLT_MAX;
        float x2 = FLT_MIN;
        float y2 = FLT_MIN;

        for (const UnityEngine::Vector3& v : screenSpaceCorners)
        {
            if (v.z <= 0.f)
                continue;

            x = Hax::Min(v.x, x);
            y = Hax::Min(v.y, y);
            x2 = Hax::Max(v.x, x2);
            y2 = Hax::Max(v.y, y2);
        }

        if (y2 - y < 15.f)
            y2 = y + 15.f;

        return UnityEngine::Rect::MinMaxRect(x, y, x2, y2);
    }
}
