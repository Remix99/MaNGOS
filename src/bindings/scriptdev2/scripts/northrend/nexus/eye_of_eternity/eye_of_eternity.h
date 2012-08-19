
#ifndef DEF_EYE_OF_ETERNITY_H
#define DEF_EYE_OF_ETERNITY_H

enum
{
    MAX_ENCOUNTER = 1,

    TYPE_MALYGOS = 0,

    NPC_MALYGOS = 28859,

    // ******************************** Items, NPCs & GameObjects ******************************** //

    ITEM_KEY_TO_FOCUSING_IRIS       = 44582,
    ITEM_KEY_TO_FOCUSING_IRIS_H     = 44581,

    GO_PLATFORM                     = 193070,
    GO_EXIT_PORTAL                  = 193908,
    GO_FOCUSING_IRIS                = 193958,
    GO_FOCUSING_IRIS_H              = 193960,
    GO_ALEXSTRASZAS_GIFT            = 193905,
    GO_ALEXSTRASZAS_GIFT_H          = 193967,
    GO_HEART_OF_MAGIC               = 194158,
    GO_HEART_OF_MAGIC_H             = 194159,

    //////////////// PHASE 1 ////////////////
    NPC_VORTEX                      = 30090,
    NPC_POWER_SPARK                 = 30084,
    NPC_SPARK_PORTAL                = 30118, // for power sparks
    VEHICLE_VORTEX                  = 168,

    //////////////// PHASE 2 ////////////////
    NPC_HOVER_DISK_MELEE            = 30234,
    NPC_HOVER_DISK_CASTER           = 30248,
    NPC_NEXUS_LORD                  = 30245, // 2 (normal) and 4 (heroic) of them are spawned on beginning of phase 2
    NPC_SCION_OF_ETERNITY           = 30249, // 4 (normal) and 8 (heroic) of them are spawned on beginning of phase 2
    NPC_ARCANE_OVERLOAD             = 30282,
    NPC_SURGE_OF_POWER              = 30334,

    //////////////// PHASE 3 ////////////////
    NPC_STATIC_FIELD                = 30592, // trigger for that spell
    NPC_WYRMREST_SKYTALON           = 30161, // dragon vehicle in 3-d Phase
    NPC_ALEXSTRASZA                 = 32295,
    NPC_ALEXSTRASZAS_GIFT           = 32448,
    NPC_ALEXSTRASZA_THE_LIFE_BINDER = 31253, // maybe trigger npc

    // ********************************* Some Spells *************************************//
    SPELL_VORTEX_RIDE_AURA          = 56266, // on 10 sec, deal 2000 damage all player around caster
    SPELL_VORTEX_VISUAL             = 55873, // visual effect around platform
    SPELL_DESTROY_PLATFORM_PRE      = 58842, // lights all over the platform
    SPELL_DESTROY_PLATFORM_BOOM     = 59084, // Big Blue boom
};

struct MANGOS_DLL_DECL instance_eye_of_eternity : public ScriptedInstance
{
    public:
        instance_eye_of_eternity(Map* pMap);

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);
        bool IsEncounterInProgress() const;

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() {   return strInstData.c_str();}
        void Load(const char* chrIn);

        void DespawnCreatures(uint32 uiEntry);

        void ActivateVisualOfVortex();
        void DestroyVisualOfVortex(bool boom);
        void HandleRiderOfVortex(Unit* pTarget);

        bool IsAnyAddAtLife();

    private:
        std::string strInstData;
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        Difficulty difficulty;

        GuidList m_lPowerSparkList;
        GuidList m_lArcaneOverload;
        GuidList m_lNexusLord;
        GuidList m_lScionOfEternity;
        GuidList m_lHoverDisk;
        GuidList m_lStaticField;

        GuidList m_lSparkPortal;
        GuidList m_lVortex;

};
#endif
