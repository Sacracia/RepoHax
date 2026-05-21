#pragma once

#include <third_party/haxsdk/hax.h>

#define LOC_LIST(X) \
X(LocKey_Stats, L"Stats", L"Статы") \
X(LocKey_Entities, L"Entities", L"Существа") \
X(LocKey_Vision, L"Vision", L"Зрение") \
X(LocKey_Valuables, L"Valuables", L"Ценности") \
X(LocKey_Items, L"Items", L"Предметы") \
X(LocKey_Level, L"Level", L"Уровень") \
X(LocKey_Misc, L"Miscellaneous", L"Разное")\
X(LocKey_Settings, L"Settings", L"Настройки")\
X(LocKey_HEALTH, L"HEALTH", L"ЗДОРОВЬЕ") \
X(LocKey_Godmode, L"Godmode", L"Бессмертие") \
X(LocKey_HealToMax, L"Heal to max", L"Исцелиться на максимум") \
X(LocKey_MOVEMENT, L"MOVEMENT", L"ПЕРЕДВИЖЕНИЕ") \
X(LocKey_InfiniteStamina, L"Infinite stamina", L"Стамина не кончается") \
X(LocKey_Default, L"Default", L"Обычная") \
X(LocKey_WalkingSpeed, L"Walking speed", L"Скорость ходьбы") \
X(LocKey_RunningSpeed, L"Running speed", L"Скорость бега") \
X(LocKey_CrouchingSpeed, L"Crouching speed", L"Скорость ползания") \
X(LocKey_InfiniteJumps, L"Infinite jumps", L"Бесконечные прыжки") \
X(LocKey_DontTumble, L"Dont tumble", L"Не падать") \
X(LocKey_DisableTumbling, L"Disable tumbling from external sources", L"Отключает падение от внешних источников") \
X(LocKey_GRAB, L"GRAB", L"ЗАХВАТ") \
X(LocKey_HighStrength, L"High strength", L"Большая сила") \
X(LocKey_AvailableIfHost, L"Available if you are host", L"Доступно если вы хост") \
X(LocKey_UnlimitedRange, L"Unlimited range", L"Неограниченная дальность") \
X(LocKey_NoOvercharge, L"No overcharge", L"Нет перегрузки") \
X(LocKey_POWERUPS, L"POWERUPS", L"УЛУЧШЕНИЯ") \
X(LocKey_AutoApplyUpgrades, L"Automatically apply upgrades", L"Автоматически применять улучшения") \
X(LocKey_ManualApplication, L"Manual application", L"Ручное применение") \
X(LocKey_HEALTHPACKS, L"HEALTH PACKS", L"АПТЕЧКИ") \
X(LocKey_ApplyAidKit, L"Apply an available aid kit to yourself", L"Применить на себя доступную аптечку") \
X(LocKey_VIEW, L"VIEW", L"ОБЗОР") \
X(LocKey_BetterVision, L"Better vision", L"Улучшенное зрение") \
X(LocKey_ThirdPerson, L"Third person", L"Третье лицо") \
X(LocKey_DefaultView, L"Default", L"Обычное") \
X(LocKey_Degrees, L"%d degrees", L"%d градусов") \
X(LocKey_FieldOfView, L"Field of view", L"Поле зрения") \
X(LocKey_FLASHLIGHT, L"FLASHLIGHT", L"ФОНАРИК") \
X(LocKey_LightIntensity, L"Light intensity", L"Яркость света") \
X(LocKey_IlluminationAngle, L"Illumination angle", L"Угол освещения") \
X(LocKey_WorksWhileCrouching, L"Works while crouching", L"Работает в приседе") \
X(LocKey_DEADHEAD, L"DEAD HEAD", L"МЕРТВАЯ ГОЛОВА") \
X(LocKey_MaxBatteryHead, L"Max battery", L"Полный заряд") \
X(LocKey_VISUALS, L"VISUALS", L"ВИЗУАЛ") \
X(LocKey_DisplayThroughWalls, L"Display through walls", L"Отображать через стены") \
X(LocKey_DisplayBoxesThroughWalls, L"Display boxes through walls", L"Отображать ящики через стены") \
X(LocKey_WithinSelectedDistance, L"Within selected distance", L"В пределах выбранной дистанции") \
X(LocKey_DisplayDistance, L"Display distance", L"Дистанция отображения") \
X(LocKey_XrayHighlight, L"X-ray hightlight", L"Рентген подсветка") \
X(LocKey_NotAllHighlighted, L"Some objects are not highlighted", L"Некоторые предметы не подсвечиваются") \
X(LocKey_VALUE, L"VALUE", L"ЦЕНА") \
X(LocKey_NoDamageValuables, L"Valuables dont get damage", L"Ценности не получают повреждений") \
X(LocKey_ChangeValue, L"Change value of all valuables:", L"Изменить цену всех ценностей:") \
X(LocKey_ToZero, L"To zero", L"На ноль") \
X(LocKey_ToMax, L"To max", L"На максимальную") \
X(LocKey_EXTRACTION, L"EXTRACTION POINTS", L"ТОЧКИ СБОРА") \
X(LocKey_ActivateNext, L"Activate next", L"Активировать следующую точку") \
X(LocKey_ENEMIES, L"ENEMIES", L"ВРАГИ") \
X(LocKey_Spawn, L"Spawn", L"Заспавить") \
X(LocKey_SpawnAlt, L"Spawn", L"Заспавнить") \
X(LocKey_CantSeePlayers, L"Cant see players", L"Не видят игроков") \
X(LocKey_NoGrabLimit, L"No grab time limitation", L"Можно держать вечно") \
X(LocKey_PLAYERS, L"PLAYERS", L"ИГРОКИ") \
X(LocKey_Tumble, L"Tumble", L"Уронить") \
X(LocKey_Kill, L"Kill", L"Убить") \
X(LocKey_HostOnly, L"Host only", L"Только хост") \
X(LocKey_Revive, L"Revive", L"Воскресить") \
X(LocKey_VoiceChat, L"Voice chat", L"Голосовой чат") \
X(LocKey_Alive, L"Alive", L"Живых") \
X(LocKey_Dead, L"Dead", L"Мертвых") \
X(LocKey_GENERAL, L"GENERAL", L"ОБЩЕЕ") \
X(LocKey_LevelsCompleted, L"Number of levels completed", L"Количество пройденных уровней") \
X(LocKey_NextLevel, L"Go next level", L"Перейти на следующий уровень") \
X(LocKey_MAPFILTER, L"MAP FILTER", L"ФИЛЬТР КАРТ") \
X(LocKey_MapFilterDesc, L"Select maps that will not be set. Available if you are host", L"Выберите карты которые не будут выпадать. Доступно если вы хост") \
X(LocKey_GUNS, L"GUNS", L"ОРУЖИЕ") \
X(LocKey_MaxBatteryGun, L"Max battery", L"Максимальная батарея") \
X(LocKey_LaserCrosshair, L"Laser crosshair", L"Лазерный прицел") \
X(LocKey_SPAWN, L"SPAWN", L"СПАВН") \
X(LocKey_TRUCK, L"TRUCK", L"ФУРГОН") \
X(LocKey_TeleportTruck, L"Teleport to truck", L"Телепортироваться в фургон") \
X(LocKey_Cosmetic, L"Cosmetic", L"Косметика") \
X(LocKey_Common, L"Common", L"Обычная") \
X(LocKey_Uncommon, L"Uncommon", L"Необычная") \
X(LocKey_Rare, L"Rare", L"Редкая") \
X(LocKey_UltraRare, L"Ultra rare", L"Очень редкая") \
X(LocKey_SETTINGS, L"SETTINGS", L"НАСТРОЙКИ") \
X(LocKey_Language, L"Language", L"Язык") \
X(LocKey_Hotkey, L"Hotkey", L"Горячая клавиша") \
X(LocKey_OpenCloseMenu, L"Open / close menu", L"Открыть / закрыть меню") \
X(LocKey_DarkenBackground, L"Darken the screen behind the menu", L"Затемнять экрана позади меню")\
X(LocKey_CrouchRest, L"Crouch rest", L"Восст. в приседе") \
X(LocKey_ExtraJump, L"Extra jump", L"Доп. прыжок") \
X(LocKey_Range, L"Range", L"Дальность") \
X(LocKey_SprintSpeed, L"Sprint speed", L"Скорость бега") \
X(LocKey_Stamina, L"Stamina", L"Выносливость") \
X(LocKey_Strength, L"Strength", L"Сила") \
X(LocKey_Launch, L"Launch", L"Запуск") \
X(LocKey_Wings, L"Wings", L"Крылья") \
X(LocKey_Health, L"Health", L"Здоровье") \
X(LocKey_PlayersCount, L"Players count", L"Счетчик игроков") \
X(LocKey_Climb, L"Climb", L"Карабканье") \
X(LocKey_HeadBattery, L"Head battery", L"Заряд головы") \
X(LocKey_Small, L"Small", L"Маленькая") \
X(LocKey_Medium, L"Medium", L"Средняя") \
X(LocKey_Large, L"Large", L"Большая") \
X(LocKey_Unknown, L"Unknown", L"Неизвестное") \
X(LocKey_UnlockAll, L"Unlock all", L"Открыть все")\
X(LocKey_ResetAll, L"Reset all", L"Сбросить все")

namespace Cheat
{
    enum LocKey
    {
        #define X(key, en, ru) key,
        LOC_LIST(X)
        #undef X
        LocKey_Count
    };

    enum Lang
    {
        Lang_Eng,
        Lang_Ru,
        Lang_Count
    };

    inline constexpr Hax::WStringView g_LocDict[Lang_Count][LocKey_Count] = 
    {
        {
            #define X(key, en, ru) en,
            LOC_LIST(X)
            #undef X
        },
        {
            #define X(key, en, ru) ru,
            LOC_LIST(X)
            #undef X
        }
    };
}