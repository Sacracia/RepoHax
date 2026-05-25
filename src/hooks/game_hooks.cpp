#include <src/pch.h>

#include <src/cheat.h>
#include <src/game_classes.h>
#include <src/resource.h>
#include <src/overlay/esp/esp.h>

namespace Cheat
{
    extern void Hook(void* ptr, void* detour, SafetyHookInline& out, const char* name);
    extern void HookModuleProc(HMODULE module, const char* procName, void* procHook, SafetyHookInline& out);

    static void* Hooked_MonoRuntimeInvoke(UVM::Method* a1, void* a2, void* a3, void* a4);
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
    static void SpawnCosmeticBox(int rarity);
    static void ParseEnemies();
    static void ParseItems();
    static void ParseLevels();
    static void DrawPlayerChams(PlayerAvatar player, UnityEngine::CommandBuffer cb, UnityEngine::Material aliveMat, UnityEngine::Material deadMat);

    void HookMonoRuntimeInvoke()
    {
        HookModuleProc((HMODULE)Hax::Unity::GetUvmHandle(), "mono_runtime_invoke", Hooked_MonoRuntimeInvoke, G->MonoRuntimeInvokeHook);
    }

    void HookGameFuncs()
    {
        Hax::LogDebug(G->Logger, L"Installing game hooks...");

        #define HOOK(ptr, n) Hook(ptr, Hooked__ ## n, G-> ## n ## _Hook, #n)
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

    void HookPlayerLoop()
    {
        HAX_PANIC(G->PlayerLoop.ReadyToHook, &G->Logger, L"LoopSystem not ready");

        Hook(G->PlayerLoop.UpdatePtr, Hooked__UpdateLoop, G->UpdateHook, "LoopSystem.Update");
        Hook(G->PlayerLoop.PostLateUpdatePtr, Hooked__PostLateUpdateLoop, G->PostLateUpdateHook, "LoopSystem.PostLateUpdate");
        Hook(G->PlayerLoop.PresentAfterDrawPtr, Hooked__PresentAfterDrawLoop, G->PresentAfterDrawHook, "LoopSystem.PresentAfterDrawLoop");
    }

    static void* Hooked_MonoRuntimeInvoke(UVM::Method* a1, void* a2, void* a3, void* a4)
    {
        void* ret = G->MonoRuntimeInvokeHook.unsafe_ccall<void*, void*, void*, void*, void*>(a1, a2, a3, a4);

        if (G->UnityLoadedEvent != nullptr)
        {
            Hax::StringView methodName = UVM::MethodGetName(*a1);
            if (methodName == "Update")
            {
                ::SetEvent(G->UnityLoadedEvent);
                G->UnityLoadedEvent = nullptr;
            }
        }

        return ret;
    }

    static void Hooked__EventSystem_Update(UnityEngine::EventSystem __this)
    {
        try
        {
            if (__this != UnityEngine::EventSystem::GetCurrent())
                return;

            PlayerAvatar avatar = PlayerAvatar::instance();
            PlayerHealth health = avatar ? avatar.playerHealth() : nullptr;

            G->IsClient = !SemiFunc::IsMasterClientOrSingleplayer();
            G->IsInGame = IsInGame();

            if (!G->PlayerLoop.ReadyToHook)
            {
                G->PlayerLoop.UpdatePtr = GetPlayerLoopPtr(UnityEngine::PlayerLoop::Update::typeof(), UnityEngine::PlayerLoop::ScriptRunBehaviourUpdate::typeof());
                Hax::LogDebug(G->Logger, L"m_UpdatePtr ptr = %p", G->PlayerLoop.UpdatePtr);

                G->PlayerLoop.PostLateUpdatePtr = GetPlayerLoopPtr(UnityEngine::PlayerLoop::PostLateUpdate::typeof(), UnityEngine::PlayerLoop::PlayerSendFrameStarted::typeof());
                Hax::LogDebug(G->Logger, L"m_PostLateUpdatePtr ptr = %p", G->PlayerLoop.PostLateUpdatePtr);

                G->PlayerLoop.PresentAfterDrawPtr = GetPlayerLoopPtr(UnityEngine::PlayerLoop::PostLateUpdate::typeof(), UnityEngine::PlayerLoop::PresentAfterDraw::typeof());
                Hax::LogDebug(G->Logger, L"m_PresentAfterDrawPtr ptr = %p", G->PlayerLoop.PresentAfterDrawPtr);

                G->PlayerLoop.ReadyToHook = true;
            }

            if (G->HealToMax)
            {
                G->HealToMax = false;
                if (health)
                    health.HealOther(99999, true);
            }

            if (G->UpgradeToToggle)
            {
                ItemUpgrade item = G->UpgradeToToggle;
                G->UpgradeToToggle = nullptr;

                ItemToggle toggle = item.itemToggle();
                if (toggle)
                {
                    toggle.photonView() = item.photonView();
                    toggle.ToggleItem(true, SemiFunc::PhotonViewIDPlayerAvatarLocal());
                }
            }

            if (G->AidToToggle)
            {
                ItemHealthPack item = G->AidToToggle;
                G->AidToToggle = nullptr;

                ItemToggle toggle = item.itemToggle();
                if (toggle)
                {
                    toggle.photonView() = item.photonView();
                    toggle.ToggleItem(true, SemiFunc::PhotonViewIDPlayerAvatarLocal());
                }
            }

            if (G->EnemiesPool.Empty())
                ParseEnemies();

            if (G->ItemsPool.Empty())
                ParseItems();

            if (G->LevelBans.Empty())
                ParseLevels();

            if (G->EnemyToSpawn)
            {
                EnemySetup enemy = G->EnemyToSpawn;
                G->EnemyToSpawn = nullptr;

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

            if (!G->CommandBuffer)
            {
                G->CommandBuffer = UnityEngine::CommandBuffer::New();
                UVM::GCHandleNew(*G->CommandBuffer.GetPtr(), true);
            }

            if (!G->Bundle)
            {
                auto ptr = Hax::Gui::GetResourceData((Hax::Handle)G->Handle, IDR_BUNDLE1, L"BUNDLE");

                System::Array<char> bundleRaw{UVM::ArrayNew(UVM::TypeGetClass(*System::Char::typeof().GetPtr()->Type), ptr.Size())};

                memcpy(bundleRaw.GetPtr()->begin(), ptr.Data(), ptr.Size());
                G->Bundle = UnityEngine::AssetBundle::LoadFromMemory(bundleRaw);
                G->Bundle.SetHideFlags(UnityEngine::HideFlags::DontUnloadUnusedAsset);

                Hax::LogDebug(G->Logger, L"Bundle loaded");
            }

            if (!G->ItemsChamsMat && G->Bundle)
            {
                UnityEngine::Shader shader{G->Bundle.LoadAsset(System::String::New("assets/myshader.shader"), UnityEngine::Shader::typeof()).GetPtr()};

                G->ItemsChamsMat = UnityEngine::Material::New(shader);
                G->ItemsChamsMat.SetHideFlags(UnityEngine::HideFlags::DontUnloadUnusedAsset);
                G->ItemsChamsMat.SetColor(UnityEngine::Color(1.f, 1.f, 0.9f, 1.f));
                UVM::GCHandleNew(*G->ItemsChamsMat.GetPtr(), true);
                Hax::LogDebug(G->Logger, L"Items chams mat loaded");

                G->PlayerChamsMat = UnityEngine::Material::New(shader);
                G->PlayerChamsMat.SetHideFlags(UnityEngine::HideFlags::DontUnloadUnusedAsset);
                G->PlayerChamsMat.SetColor(UnityEngine::Color::green());
                UVM::GCHandleNew(*G->PlayerChamsMat.GetPtr(), true);
                Hax::LogDebug(G->Logger, L"Player chams mat loaded");

                G->HeadChamsMat = UnityEngine::Material::New(shader);
                G->HeadChamsMat.SetHideFlags(UnityEngine::HideFlags::DontUnloadUnusedAsset);
                G->HeadChamsMat.SetColor(UnityEngine::Color(0x54 / 255.f, 0x54 / 255.f, 0x54 / 255.f, 1.f));
                UVM::GCHandleNew(*G->HeadChamsMat.GetPtr(), true);
                Hax::LogDebug(G->Logger, L"Head chams mat loaded");
            }

            if (G->SetToZero)
            {
                G->SetToZero = false;
                if (ValuableDirector dir = ValuableDirector::instance())
                {
                    for (ValuableObject obj : dir.valuableList())
                    {
                        if (obj)
                            Heal(obj.physGrabObject().impactDetector(), -999999.f);
                    }
                }
            }

            if (G->SetToMax)
            {
                G->SetToMax = false;
                if (ValuableDirector dir = ValuableDirector::instance())
                {
                    for (ValuableObject obj : dir.valuableList())
                    {
                        if (obj)
                            Heal(obj.physGrabObject().impactDetector(), 999999.f);
                    }
                }
            }

            if (G->ItemToSpawn)
            {
                Item item = G->ItemToSpawn;
                G->ItemToSpawn = nullptr;
                SpawnItem(item);
            }

            if (G->ForceNextLevel)
            {
                G->ForceNextLevel = false;
                if (RunManager manager = RunManager::instance())
                    manager.ChangeLevel(true, false);
            }

            if (G->ActivateNextPoint)
            {
                G->ActivateNextPoint = false;
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

            if (G->TeleportToTruck)
            {
                G->TeleportToTruck = false;
                if (avatar && !avatar.deadSet())
                    avatar.playerTransform().SetPosition(TruckSafetySpawnPoint::instance().GetTransform().GetPosition());
            }

            if (G->PlayerToTumble)
            {
                PlayerAvatar player = G->PlayerToTumble;
                G->PlayerToTumble = nullptr;
                if (player && !player.isDisabled())
                    player.tumble().TumbleRequest(true, false);
            }

            if (G->PlayerToKill)
            {
                PlayerAvatar player = G->PlayerToKill;
                G->PlayerToKill = nullptr;
                if (player && !player.deadSet() && !G->IsClient)
                    player.PlayerDeath(-1);
            }

            if (G->PlayerToRevive)
            {
                PlayerAvatar player = G->PlayerToRevive;
                G->PlayerToRevive = nullptr;
                if (player && player.deadSet() && !G->IsClient)
                    player.Revive(false);
            }

            PlayerVoiceChat chat = avatar ? avatar.voiceChat() : nullptr;
            if (chat && avatar.voiceChatFetched() && G->ChatPrefered != ChatPref::None)
            {
                bool isDeadPrefered = G->ChatPrefered == ChatPref::Dead;
                if (chat.inLobbyMixer() != isDeadPrefered)
                {
                    chat.ToggleMixer(isDeadPrefered, false);
                    Hax::LogDebug(G->Logger, L"Chat changed");
                }
                G->ChatPrefered = ChatPref::None;
            }

            if (G->UnlockAllCosmetic && MetaManager::instance())
            {
                G->UnlockAllCosmetic = false;
                MetaManager::instance().CosmeticUnlockAll();
            }

            if (G->ResetAllCosmetic && MetaManager::instance())
            {
                G->ResetAllCosmetic = false;
                MetaManager::instance().Reset();
            }

            if (G->RarityToSpawn > 0)
            {
                int rarity = G->RarityToSpawn;
                G->RarityToSpawn = 0;
                SpawnCosmeticBox(rarity - 1);
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->EventSystem_Update_Hook.unsafe_call<void, UnityEngine::EventSystem>(__this);
    }

    static void Hooked__PlayerHealth_Hurt(PlayerHealth __this, int damage, bool savingGrace, int enemyIndex)
    {
        if (G->Godmode)
            return;

        G->PlayerHealth_Hurt_Hook.unsafe_call<void, PlayerHealth, int, bool, int>(__this, damage, savingGrace, enemyIndex);
    }

    static void Hooked__PlayerAvatar_PlayerDeath(PlayerAvatar __this, int enemyIndex)
    {
        if (G->Godmode && __this.isLocal())
            return;

        G->PlayerAvatar_PlayerDeath_Hook.unsafe_call<void, PlayerAvatar, int>(__this, enemyIndex);
    }

    static void Hooked__PlayerController_FixedUpdate(PlayerController __this)
    {
        try
        {
            float& walkSpeed = __this.MoveSpeed();
            float& sprintSpeed = __this.SprintSpeed();
            float& crouchSpeed = __this.CrouchSpeed();

            float backup[3] = { walkSpeed, sprintSpeed, crouchSpeed };

            if (G->InfStamina)
                __this.EnergyCurrent() = __this.EnergyStart();

            walkSpeed *= (float)G->Accel.Walking;
            sprintSpeed *= (float)G->Accel.Sprinting;
            crouchSpeed *= (float)G->Accel.Crouching;

            G->PlayerController_FixedUpdate_Hook.unsafe_call<void, PlayerController>(__this);

            walkSpeed = backup[0];
            sprintSpeed = backup[1];
            crouchSpeed = backup[2];
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");

            G->PlayerController_FixedUpdate_Hook.unsafe_call<void, PlayerController>(__this);
        }
    }

    static void Hooked__PlayerController_Update(PlayerController __this)
    {
        int& jumps = __this.JumpExtra();
        int cachedJumps = jumps;
        if (G->InfJumps)
            jumps = 9999;

        G->PlayerController_Update_Hook.unsafe_call<void, PlayerController>(__this);

        jumps = cachedJumps;
    }

    static void Hooked__PlayerTumble_TumbleRequest(PlayerTumble __this, bool isTumbling, bool playerInput)
    {
        if (G->NoTumble && __this.playerAvatar().isLocal() && !playerInput)
            return;

        G->PlayerTumble_TumbleRequest_Hook.unsafe_call<void, PlayerTumble, bool, bool>(__this, isTumbling, playerInput);
    }

    static void Hooked__FlashlightController_Update(FlashlightController __this)
    {
        try
        {
            PlayerAvatar avatar = PlayerAvatar::instance();

            bool wasCrouching = false;
            if (G->Flashlight.InCrouch && avatar)
            {
                wasCrouching = avatar.isCrouching();
                avatar.isCrouching() = false;
            }

            G->FlashlightController_Update_Hook.unsafe_call<void, FlashlightController>(__this);

            if (G->Flashlight.InCrouch && avatar)
                avatar.isCrouching() = wasCrouching;

            UnityEngine::Light spotLight = __this.spotlight();
            if ((float)G->Flashlight.Angle != spotLight.GetSpotAngle())
            {
                spotLight.SetSpotAngle((float)G->Flashlight.Angle);
            }

            spotLight.SetIntensity(spotLight.GetIntensity() * (G->Flashlight.Intensity / 10.f));
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");

            G->FlashlightController_Update_Hook.unsafe_call<void, FlashlightController>(__this);
        }
    }

    static UpgradeData* GetUpgradeInfoFromName(System::String name)
    {
        for (int i = 0; i < (int)UpgradeType::N; ++i)
        {
            if (name.ToView().EndsWith(G->UpgradesData[i].InternalNameEnd))
                return &G->UpgradesData[i];
        }
        return nullptr;
    }

    static void Hooked__ItemUpgrade_Update(ItemUpgrade __this)
    {
        try
        {
            G->UpgradesPool.GetBack().PushBack(__this);

            if (G->AutoUseUpgrs)
            {
                UpgradeData* data = GetUpgradeInfoFromName(__this.itemAttributes().itemAssetName().GetPtr());
                if (data != nullptr && data->AutoUse)
                {
                    ItemToggle toggle = __this.itemToggle();
                    if (toggle)
                    {
                        toggle.photonView() = __this.photonView();
                        toggle.ToggleItem(true, SemiFunc::PhotonViewIDPlayerAvatarLocal());
                    }
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->ItemUpgrade_Update_Hook.unsafe_call<void, ItemUpgrade>(__this);
    }

    static void Hooked__ItemHealthPack_Update(ItemHealthPack __this)
    {
        if (!RunManager::instance().levelIsShop())
            G->AidsPool.GetBack().PushBack(__this);

        G->ItemHealthPack_Update_Hook.unsafe_call<void, ItemHealthPack>(__this);
    }

    static void Hooked__UpdateLoop()
    {
        G->UpgradesPool.GetBack().Clear();
        G->AidsPool.GetBack().Clear();

        G->UpdateHook.unsafe_call<void>();

        G->UpgradesPool.RefreshSpare();
        G->AidsPool.RefreshSpare();
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
        return G->IsInGame && MenuManager::instance() && !MenuManager::instance().currentMenuPage();
    }

    static float prevPlane;
    static float prevFov;
    static void Hooked__PostLateUpdateLoop()
    {
        try
        {
            UnityEngine::Camera mainCam = SemiFunc::MainCamera();
            PlayerAvatar avatar = PlayerAvatar::instance();
            if (mainCam && G->IsInGame && avatar)
            {
                if (G->BetterVision)
                {
                    prevPlane = mainCam.GetFarClipPlane();
                    if (prevPlane < 32.f)
                        mainCam.SetFarClipPlane(32.f);
                }

                prevFov = mainCam.GetFieldOfView();
                if (prevFov < (float)G->Fov)
                    mainCam.SetFieldOfView((float)G->Fov);

                if (G->ThirdPerson && !SpectateCamera::instance())
                    SetThirdPerson();

                if (ShouldDrawEsp())
                {
                    G->PixelHeight = (float)mainCam.GetPixelHeight();
                    G->PixelWidth = (float)mainCam.GetPixelWidth();
                    G->ScreenHeight = (float)UnityEngine::Screen::GetHeight();
                    G->ScreenWidth = (float)UnityEngine::Screen::GetWidth();

                    if (G->EnemiesEsp)
                    {
                        auto& back = G->EnemiesEspBuffer.GetBack();
                        back.Clear();
                        if (EnemyDirector dir = EnemyDirector::instance())
                        {
                            for (EnemyParent parent : dir.enemiesSpawned())
                            {
                                Enemy enemy = parent.enemy();
                                if (enemy && parent.Spawned())
                                {
                                    Hax::Optional<EnemyEspData> data = ParseEnemyEspData(enemy);
                                    if (data.HasValue())
                                        back.PushBack(*data);
                                }
                            }
                        }
                        G->EnemiesEspBuffer.RefreshSpare();
                    }

                    if (G->ValuablesEsp)
                    {
                        auto& back = G->ValuablesEspBuffer.GetBack();
                        back.Clear();
                        if (ValuableDirector dir = ValuableDirector::instance())
                        {
                            for (ValuableObject obj : dir.valuableList())
                            {
                                Hax::Optional<ValuableEspData> data = ParseValuableEspData(obj);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        G->ValuablesEspBuffer.RefreshSpare();
                    }

                    if (G->ExtrPointsEsp)
                    {
                        auto& back = G->ExtrPointsEspBuffer.GetBack();
                        back.Clear();
                        if (RoundDirector dir = RoundDirector::instance())
                        {
                            for (UnityEngine::GameObject go : dir.extractionPointList())
                            {
                                Hax::Optional<ExtrPointEspData> data = ParseExtrPointEspData(go);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        G->ExtrPointsEspBuffer.RefreshSpare();
                    }

                    if (G->TruckEsp)
                    {
                        auto& back = G->TruckEspBuffer.GetBack();
                        back.Clear();
                        if (TruckSafetySpawnPoint truck = TruckSafetySpawnPoint::instance())
                        {
                            Hax::Optional<TruckEspData> data = ParseTruckEspData(truck);
                            if (data.HasValue())
                                back.PushBack(*data);
                        }
                        G->TruckEspBuffer.RefreshSpare();
                    }

                    if (G->PlayersEsp)
                    {
                        auto& back = G->PlayersEspBuffer.GetBack();
                        back.Clear();
                        if (GameDirector dir = GameDirector::instance())
                        {
                            for (PlayerAvatar avatar : dir.PlayerList())
                            {
                                Hax::Optional<PlayerEspData> data = ParsePlayerEspData(avatar);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        G->PlayersEspBuffer.RefreshSpare();
                    }

                    if (G->CosmeticBoxesEsp)
                    {
                        auto& back = G->CosmeticBoxesEspBuffer.GetBack();
                        back.Clear();
                        if (RoundDirector dir = RoundDirector::instance())
                        {
                            for (CosmeticWorldObject box : dir.cosmeticWorldObjects())
                            {
                                Hax::Optional<CosmeticBoxEspData> data = ParseCosmeticBoxEspData(box);
                                if (data.HasValue())
                                    back.PushBack(*data);
                            }
                        }
                        G->CosmeticBoxesEspBuffer.RefreshSpare();
                    }
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->PostLateUpdateHook.unsafe_call<void>();
    }

    static void Hooked__PresentAfterDrawLoop()
    {
        try
        {
            UnityEngine::Camera mainCam = SemiFunc::MainCamera();
            if (mainCam && G->IsInGame)
            {
                if (G->BetterVision)
                    mainCam.SetFarClipPlane(prevPlane);

                mainCam.SetFieldOfView(prevFov);

                if (G->ThirdPerson && !SpectateCamera::instance())
                    mainCam.GetTransform().SetLocalPosition(UnityEngine::Vector3::zero());
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->PresentAfterDrawHook.unsafe_call<void>();
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
                    if (G->EasyGrab && backup < 20.f)
                        grabber.grabStrength() = 20.f;

                    G->PhysGrabObject_PhysicsGrabbingManipulation_Hook.unsafe_call<void, PhysGrabObject>(__this);

                    grabber.grabStrength() = backup;
                    return;
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->PhysGrabObject_PhysicsGrabbingManipulation_Hook.unsafe_call<void, PhysGrabObject>(__this);
    }

    static void Hooked__PostProcessLayer_BuildCommandBuffers(UnityEngine::PostProcessLayer __this)
    {
        try
        {
            if (G->BetterVision && G->IsInGame)
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
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->PostProcessLayer_BuildCommandBuffers_Hook.unsafe_call<void, UnityEngine::PostProcessLayer>(__this);
    }

    void Hooked__PlayerAvatarVisuals_Update(PlayerAvatarVisuals __this)
    {
        try
        {
            if (GameManager gm = GameManager::instance())
            {
                PlayerAvatar avatar = __this.playerAvatar();
                if (G->ThirdPerson && avatar && !__this.isMenuAvatar() && avatar.isLocal())
                {
                    __this.showSelfOverrideTimer() = Hax::Max(0.1f, __this.showSelfOverrideTimer());
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->PlayerAvatarVisuals_Update_Hook.unsafe_call<void, PlayerAvatarVisuals>(__this);
    }

    static void Hooked__PhysGrabber_PhysGrabLogic(PhysGrabber __this)
    {
        float& range = __this.grabRange();
        float cached = range;
        PlayerAvatar playerAvatar = PlayerAvatar::instance();

        if (G->UnlimGrabRange && playerAvatar && __this == playerAvatar.physGrabber())
            range = 999999.f;

        G->PhysGrabber_PhysGrabLogic_Hook.unsafe_call<void, PhysGrabber>(__this);

        range = cached;
    }

    static bool g_RaycastMaxDist;
    static void Hooked__PhysGrabber_RayCheck(PhysGrabber __this, bool grab)
    {
        float& range = __this.grabRange();
        float cached = range;
        PlayerAvatar playerAvatar = PlayerAvatar::instance();

        if (G->UnlimGrabRange && playerAvatar && __this == playerAvatar.physGrabber()) 
        {
            range = 999999.f;
            g_RaycastMaxDist = true;
        }

        G->PhysGrabber_RayCheck_Hook.unsafe_call<void, PhysGrabber, bool>(__this, grab);

        range = cached;
        g_RaycastMaxDist = false;
    }

    static bool Hooked__Physics_Raycast(void* v1, void* v2, void* v3, float maxDistance, int v5, int v6)
    {
        if (g_RaycastMaxDist)
            maxDistance = 999999.f;

        return G->Physics_Raycast_Hook.unsafe_call<bool, void*, void*, void*, float, int, int>(v1, v2, v3, maxDistance, v5, v6);
    }

    static void Hooked__PhysGrabObjectImpactDetector_Break(PhysGrabObjectImpactDetector __this, float a1, void* a2, int a3, bool a4)
    {
        if (G->Unbreakable && __this.isValuable())
            return;

        G->PhysGrabObjectImpactDetector_Break_Hook.unsafe_call<void, PhysGrabObjectImpactDetector, float, void*, int, bool>(__this, a1, a2, a3, a4);
    }

    static void Hooked__PhysGrabObjectImpactDetector_DestroyObject(PhysGrabObjectImpactDetector __this, bool a1)
    {
        if (G->Unbreakable && __this.isValuable())
            return;

        G->PhysGrabObjectImpactDetector_DestroyObject_Hook.unsafe_call<void, PhysGrabObjectImpactDetector, bool>(__this, a1);
    }

    static bool g_PretendMaster;
    static bool Hooked__PhotonNetwork_IsMasterClient()
    {
        if (g_PretendMaster)
        {
            g_PretendMaster = false;
            return true;
        }
        return G->PhotonNetwork_IsMasterClient_Hook.unsafe_call<bool>();
    }

    static void Hooked__ItemBattery_Update(ItemBattery __this)
    {
        try
        {
            if (G->InfBattery && __this.batteryLifeInt() < __this.batteryBars() && __this.physGrabObject().grabbedLocal())
            {
                //g_PretendMaster = true;
                __this.BatteryFullPercentChange(__this.batteryBars(), false);
                //g_PretendMaster = false;
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->ItemBattery_Update_Hook.unsafe_call<void, ItemBattery>(__this);
    }

    static void Hooked__Camera_FireOnPreRender(UnityEngine::Camera __this)
    {
        try
        {
            if (IsInGame() && __this == GameDirector::instance().MainCamera() && G->ItemsChamsMat && G->CommandBuffer)
            {
                G->CommandBuffer.Clear();

                static void* s_PrevCam;
                if (__this && __this.GetPtr()->m_CachedPtr != s_PrevCam)
                {
                    s_PrevCam = __this.GetPtr()->m_CachedPtr;
                    __this.AddCommandBuffer(UnityEngine::CameraEvent::AfterEverything, G->CommandBuffer);
                }

                if (G->ValuablesChams && ValuableDirector::instance())
                {
                    for (ValuableObject obj : ValuableDirector::instance().valuableList())
                    {
                        if (obj && obj.GetEnabled())
                        {
                            for (UnityEngine::MeshRenderer renderer : obj.GetTransform().GetChild(0).GetComponentsInChildren<UnityEngine::MeshRenderer>(true))
                            {
                                G->CommandBuffer.DrawRenderer(renderer, G->ItemsChamsMat);
                            }
                        }
                    }
                }

                if (G->PlayersChams && GameDirector::instance())
                {
                    for (PlayerAvatar player : GameDirector::instance().PlayerList())
                    {
                        DrawPlayerChams(player, G->CommandBuffer, G->PlayerChamsMat, G->HeadChamsMat);
                    }
                }
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->Camera_FireOnPreRender_Hook.unsafe_call<void, UnityEngine::Camera>(__this);
    }

    static void Hooked__ItemGun_Update(ItemGun __this)
    {
        try
        {
            UnityEngine::LineRenderer laser = __this.GetComponent<UnityEngine::LineRenderer>();

            if (__this.physGrabObject().grabbedLocal() && G->UseLaser)
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
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->ItemGun_Update_Hook.unsafe_call<void, ItemGun>(__this);
    }

    static void Hooked__RunManager_SetRunLevel(RunManager __this)
    {
        try
        {
            if (G->LevelBans.Size() > 0)
            {
                System::List<Level> levels = RunManager::instance().levels();

                LevelBan* levelBan = &G->LevelBans[0];
                Level prevLevel = __this.previousRunLevel();

                while (!levelBan->Allowed || ((G->LevelBans.Size() - G->TotalBans) > 1 && prevLevel && levels[levelBan->Index] == prevLevel))
                    levelBan = &G->LevelBans[rand() % G->LevelBans.Size()];

                __this.levelCurrent() = levels[levelBan->Index];
                return;
            }
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        G->RunManager_SetRunLevel_Hook.unsafe_call<void, RunManager>(__this);
    }

    static void Hooked__PhysGrabber_PhysGrabOverCharge(PhysGrabber __this, float _amount, float _multiplier)
    {
        if (G->NoOvercharge)
            return;

        G->PhysGrabber_PhysGrabOverCharge_Hook.unsafe_call<void, PhysGrabber, float, float>(__this, _amount, _multiplier);
    }

    static void Hooked__SpectateCamera_HeadEnergyLogic(SpectateCamera __this)
    {
        if (G->MaxHeadBattery)
        {
            __this.headEnergy() = 9999.f;
            __this.headEnergyEnough() = true;
        }

        G->SpectateCamera_HeadEnergyLogic_Hook.unsafe_call<void, SpectateCamera>(__this);
    }

    static void Hooked__EnemyRigidbody_FixedUpdate(EnemyRigidbody __this)
    {
        if (G->NoGrabMaxTime && SemiFunc::IsMasterClientOrSingleplayer())
        {
            __this.grabShakeReleaseTimer() = 0.f;
            __this.grabTimeCurrent() = 0.f;
        }

        G->EnemyRigidbody_FixedUpdate_Hook.unsafe_call<void, EnemyRigidbody>(__this);
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

        UnityEngine::Transform transform = camera.GetTransform();
        UnityEngine::Vector3 pos = transform.GetPosition() + transform.GetForward() * 2.f - transform.GetUp();

        if (SemiFunc::IsMasterClient())
            Photon::PhotonNetwork::InstantiateRoomObject(item.prefab().resourcePath(), pos, UnityEngine::Quaternion::identity());
        if (!SemiFunc::IsMultiplayer())
            UnityEngine::Object::Instantiate<UnityEngine::GameObject>(item.prefab().Prefab(), pos, UnityEngine::Quaternion::identity());
    }

    static void SpawnCosmeticBox(int rarity)
    {
        UnityEngine::Camera camera = SemiFunc::MainCamera();
        if (!camera)
            return;

        ValuableDirector dir = ValuableDirector::instance();
        if (!dir)
            return;

        auto setups = dir.cosmeticWorldObjectSetups();
        if (!setups || rarity < 1 || rarity >= setups.GetCount())
            return;

        CosmeticWorldObjectSetup setup = setups[rarity];
        if (!setup || !setup.prefab())
            return;

        UnityEngine::Transform transform = camera.GetTransform();
        UnityEngine::Vector3 pos = transform.GetPosition() + transform.GetForward() * 2.f - transform.GetUp();

        if (GameManager::instance().gameMode() == 0)
            UnityEngine::Object::Instantiate<UnityEngine::GameObject>(setup.prefab().Prefab(), pos, UnityEngine::Quaternion::identity());
        else
            Photon::PhotonNetwork::InstantiateRoomObject(setup.prefab().resourcePath(), pos, UnityEngine::Quaternion::identity());
    }

    static void ParseEnemies()
    {
        EnemyDirector dir = EnemyDirector::instance();
        if (!G->EnemiesPool.Empty() || !dir)
            return;

        Hax::LogDebug(G->Logger, L"Parsing enemies");
        //G->LoadRequests.reserve(50);

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
                    //G->LoadRequests.push_back(new LoadRequest(setup, req));

                    Hax::WStringView enemyName = ref.prefabName().begin() + 8;

                    EnemySetup& entry = G->EnemiesPool.FindOrAdd(enemyName);
                    if (!entry)
                    {
                        entry = setup;
                        UVM::GCHandleNew(*name.GetPtr(), true);
                        Hax::LogDebug(G->Logger, L"Enemy parsed %ls", enemyName.begin());
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
            int nItems = itemDict.Count();

            if (nItems == 0)
                manager.LoadItemsFromFolder();

            for (int i = 0; i < itemDict.Count(); ++i)
            {
                auto entry = itemDict.begin() + i;
                System::String itemName = entry->Key;

                Hax::char16* ptr = itemName.begin();
                if (wcsncmp(ptr, L"Item ", 5) == 0)
                    ptr += 5;

                Hax::WStringView name = ptr;

                if (!G->ItemsPool.Contains(name))
                {
                    if (entry->Value)
                    {
                        G->ItemsPool.Insert(name, i);
                        auto req = UnityEngine::Resources::LoadAsync(entry->Value.prefab().resourcePath());

                        UVM::GCHandleNew(*itemName.GetPtr(), true);
                        Hax::LogDebug(G->Logger, L"Item parsed %ls", ptr);
                    }
                    else
                        Hax::LogError(G->Logger, L"Unable to parse item %s", ptr);
                }
            }
        }
    }

    static void ParseLevels()
    {
        if (RunManager manager = RunManager::instance())
        {
            Hax::LogDebug(G->Logger, L"Parsing levels");

            System::List<Level> levels = manager.levels();
            G->LevelBans.Reserve(levels.GetCount());

            for (int i = 0; i < levels.GetCount(); ++i)
            {
                System::String levelName = levels[i].NarrativeName();

                LevelBan ban = {levelName.ToView(), i, true};
                G->LevelBans.PushBack(ban);

                UVM::GCHandleNew(*levelName.GetPtr(), true);
                Hax::LogDebug(G->Logger, L"Parsed level %ls", levelName.begin());
            }

            std::sort(G->LevelBans.begin(), G->LevelBans.end(), [](const LevelBan& o1, const LevelBan& o2) { return o1.Name < o2.Name; });
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
}