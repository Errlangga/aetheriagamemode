using System;
 
using SampSharp.GameMode.API.NativeObjects;
using SampSharp.GameMode.World;
using SampSharp.GameMode.SAMP;
using SampSharp.GameMode.Definitions;
using SampSharp.GameMode;
 
namespace SAMP.Include
{
    class RNPC
    {
        #region Variables
 
        static public readonly string VERSION           = "0.4.1";
        static public readonly int    BUILD             = 14;
        static public readonly string DATE              = "03.12.2014";
        static public readonly string VERSION_DLPAGE    = "www.mauzen.org/rnpc";
        static public readonly string VERSION_STARTURL  = "www.mauzen.org/rnpc/rnpc_start.php";
        static public readonly string VERSION_UPDATEURL = "www.mauzen.org/rnpc/rnpc_update.php";
 
        // Identify messages as RNPC commands
        // (alternative communication protocol for future versions)
        static public readonly int COMM_ID = 520;
        
        public enum RecordingType
        {
            None,
            Driver,
            OnFoot
        }
 
        static public readonly float SPEED_SPRINT   = (0.0095f);
        static public readonly float SPEED_RUN      = (0.0057f);
        static public readonly float SPEED_WALK     = (0.0015f);
        
        // RNPC_PEND_ACTION defines (Connection relevant states)
        enum State
        {
            ActionReady         = (-1),
            NotExisting         = (0),
            ConnectionPending   = (1),
            Connected           = (2),
            Stopping            = (4) // Stop-playback performed
        }
 
        // Adjustable stuff
        static public readonly float VEHICLE_DMG_MOD = (160.0f);  // velocity * this value = vehicle hit damage
 
        // AddPause compatibility mode
        static public readonly bool PAUSE_COMP = false;
 
        static public readonly bool DEBUG = true;
 
        class NonPlayerStruct
        {
            public State State = State.NotExisting;
            public int CurrentSlot = 0;
            public bool IsAttackable = false;
            public float Health = 100.0f;
            public bool IsDead = false;
            public BasePlayer LastAssailant = null;
            public Weapon DamageReason = Weapon.None;
            public int VehicleHit = 0;
        }
 
        static NonPlayerStruct[] NonPlayerData = new NonPlayerStruct[BasePlayer.Max];
 
        public delegate bool OnRNPCPlaybackFinished(BasePlayer npc);
        static public event OnRNPCPlaybackFinished OnPlaybackFinished;
 
        public delegate bool OnRNPCPlaybackStopped(BasePlayer npc);
        static public event OnRNPCPlaybackStopped OnPlaybackStopped;
 
        public delegate bool OnRNPCDeath(BasePlayer npc, BasePlayer assailant, Weapon reason);
        static public event OnRNPCDeath OnDeath;
 
        public delegate bool OnRNPCVehicleHit(BasePlayer npc, BasePlayer driver, BaseVehicle vehicle, int times);
        static public event OnRNPCVehicleHit OnVehicleHit;
 
        #endregion // Variables
 
        #region Functions
        
        static public bool Init()
        {
            BaseMode.Instance.PlayerConnected += (p, e) =>
            {
                OnPlayerConnect((BasePlayer)p);
            };
            BaseMode.Instance.PlayerDisconnected += (p, e) =>
            {
                OnPlayerDisconnect((BasePlayer)p, e.Reason);
            };
            BaseMode.Instance.PlayerSpawned += (p, e) =>
            {
                OnPlayerSpawn((BasePlayer)p);
            };
            BaseMode.Instance.PlayerStreamIn += (p, e) =>
            {
                OnPlayerStreamIn((BasePlayer)p, e.Player);
            };
            BaseMode.Instance.PlayerGiveDamage += (p, e) =>
            {
                OnPlayerGiveDamage((BasePlayer)p, e.OtherPlayer, e.Amount, e.Weapon, e.BodyPart);
            };
            BaseMode.Instance.PlayerCommandText += (p, e) =>
            {
                OnPlayerCommandText((BasePlayer)p, e.Text);
            };
            return true;
        }
 
        static public bool SetAutorepeat(BasePlayer npc, int slot)
        {
            return SendMessage(npc, -1, "RNPC:{0}", 110 + slot);
        }
 
        static public bool PauseRecordingPlayback(BasePlayer npc)
        {
            return SendMessage(npc, -1, "RNPC:103");
        }
 
        static public bool ResumeRecordingPlayback(BasePlayer npc)
        {
            return SendMessage(npc, -1, "RNPC:104");
        }
 
        static public bool ToggleVehicleCollisionCheck(BasePlayer npc, int slot)
        {
            return SendMessage(npc, -1, "RNPC:{0}", 115 + slot);
        }
 
        static public bool StopPlayback(BasePlayer npc)
        {
            return SendMessage(npc, -1, "RNPC:102");
        }
 
        static public bool SetShootable(BasePlayer npc, bool toggle)
        {
            var npcData = GetData(npc);
            if (npcData == null) return false;
 
            npcData.IsAttackable = toggle;
            return true;
        }
 
        static public float GetHealth(BasePlayer npc)
        {
            var npcData = GetData(npc);
            if (npcData == null) return float.NaN;
            return npcData.Health;
        }
 
        static public bool IsValid(BasePlayer npc)
        {
            var npcData = GetData(npc);
            if (npcData == null) return false;
            return true;
        }
 
        static bool SendMessage(BasePlayer npc, Color color, string text)
        {
            var npcData = GetData(npc);
            if(npcData == null) return false;
 
            if (DEBUG == true)
            {
                Console.WriteLine("RNPC %d <-- [CMD] %s", npc.Id, text);
            }
 
            npc.SendClientMessage(color, text);
            return true;
        }
        static bool SendMessage(BasePlayer npc, Color color, string format, params object[] arguments)
        {
            return SendMessage(npc, color, string.Format(format, arguments));
        }
 
        static NonPlayerStruct GetData(BasePlayer npc)
        {
            if(npc.IsNPC == false) return null;
            return NonPlayerData[npc.Id];
        }
 
        static public BasePlayer Connect(string name)
        {
            BasePlayer npc = Server.ConnectNPC(name, "RNPC");
 
            if(npc != null)
            {
                return null;
            }
 
            NonPlayerStruct npcData = new NonPlayerStruct();
 
            npcData.State = State.ConnectionPending;
 
            NonPlayerData[npc.Id] = npcData;
            return npc;
        }
 
        static public bool Move(BasePlayer npc, float x, float y, float z, float speed, int useZMap = 0)
        {
            var npcData = GetData(npc);
            if(npcData == null) return false;
 
            RecordingType type = (npc.InAnyVehicle == true) ? RecordingType.Driver : RecordingType.OnFoot;
            Vector3 position = (npc.InAnyVehicle == true) ? npc.Vehicle.Position : npc.Position;
            
            int slot = CreateBuild(npc, type, 95);
            RNPCInternal.Instance.AddMovement(position.X, position.Y, position.Z, x, y, z, speed, useZMap);
            RNPCInternal.Instance.FinishBuild();
            StartBuildPlayback(npc, slot);
            return true;
        }
 
        static public int CreateBuild(BasePlayer npc, RecordingType type, int slot = 0)
        {
            var npcData = GetData(npc);
            if (npcData == null) return -1;
 
            if(npcData.CurrentSlot == slot) {
                slot = (slot + 1) % 100;
            }
            
            RNPCInternal.Instance.CreateBuild(npc.Id, (int)type, slot);
            return slot;
        }
 
        static public bool StartPlayback(BasePlayer npc, string rec)
        {
            var npcData = GetData(npc);
            if(npcData == null) return false;
            
            SendMessage(npc, -1, "RNPC:109:%s", rec);
            return true;
        }
 
        static public bool StartBuildPlayback(BasePlayer npc, int slot = 0, BaseVehicle vehicle = null)
        {
            var npcData = GetData(npc);
            if(npcData == null) return false;
 
            // If enabled, wait a bit till the NPC actually entered the vehicle
            if(vehicle != null)
            {
                npc.PutInVehicle(vehicle, 0);
                
                Timer delayStart = new Timer(TimeSpan.FromMilliseconds(100), false);
                delayStart.Tick += (sender, e) =>
                {
                    npc.PutInVehicle(vehicle, 0);
 
                    SendMessage(npc, -1, "RNPC:101:%d", slot);
                    delayStart.Dispose();
                };
            }
 
            npcData.CurrentSlot = slot;
            SendMessage(npc, -1, "RNPC:101:%d", slot);
            return true;
        }
 
        static public bool SetHealth(BasePlayer npc, float health, BasePlayer assailant = null, Weapon reason = Weapon.Drown)
        {
            var npcData = GetData(npc);
            if(npcData == null) return false;
 
            npcData.Health = health;
 
            npcData.LastAssailant = assailant;
            npcData.DamageReason = reason;
 
            if(npcData.Health > 0.0f)
            {
                return true;
            }
 
            // Kill the NPC if hes not already dead
            if(npcData.IsDead == true)
            {
                return true;
            }
 
            // Stop playback
            StopPlayback(npc);
            // --> When stopped, apply death animation
            return true;
        }
 
        static public bool IsVehicleOnPlayer(BasePlayer npc, BaseVehicle vehicle)
        {
            if(npc.InAnyVehicle == true)
            {
                return false;
            }
            return (npc.Position.DistanceTo(vehicle.Position) <= 0.5);
        }
 
        #endregion // Functions
 
        #region "Callbacks"
 
        static public bool OnPlayerConnect(BasePlayer npc)
        {
            var npcData = GetData(npc);
            if(npcData == null) return false;
 
            npcData.State = State.Connected;
            npcData.IsAttackable = false;
            npcData.Health = 100.0f;
            npcData.IsDead = false;
            return true;
        }
 
        static public bool OnPlayerDisconnect(BasePlayer npc, DisconnectReason reason)
        {
            var npcData = GetData(npc);
            if (npcData == null) return false;
 
            npcData = null;
            return true;
        }
 
        static public bool OnPlayerSpawn(BasePlayer npc)
        {
            var npcData = GetData(npc);
            if (npcData == null) return false;
 
            npcData.Health = 100.0f;
            npcData.IsDead = false;
            return true;
        }
 
        static public bool OnPlayerGiveDamage(BasePlayer assailant, BasePlayer npc, float amount, Weapon weapon, BodyPart bodyPart)
        {
            var npcData = GetData(npc);
            if (npcData == null) return false;
            if (npcData.IsAttackable == false) return false;
 
            SetHealth(npc, npcData.Health - amount, assailant, weapon);
            return true;
        }
 
        static public bool OnPlayerStreamIn(BasePlayer npc, BasePlayer player)
        {
            var npcData = GetData(npc);
            if (npcData == null) return false;
            if (npcData.IsDead == false) return false;
            
            npc.ApplyAnimation("PED", "KO_skid_back", 4.1f, false, true, true, true, 0, true);
            return true;
        }
 
        static public bool OnPlayerCommandText(BasePlayer npc, string text)
        {
            var npcData = GetData(npc);
            if (npcData == null) return false;
 
            string command = text.Substring(1, 8);
 
            if(DEBUG == true)
            {
                Console.WriteLine("RNPC %d --> [CMD] %s", npc.Id, text);
            }
 
            // NPC finished playback
            if(command == "RNPC:002")
            {
                // Reset current slot for RNPC_CreateBuild_s
                npcData.CurrentSlot = -1;
 
                if(npcData.State != State.Stopping)
                {
                    OnPlaybackFinished.Invoke(npc);
                    return true;
                }
 
                // Call OnRNPCPlaybackStopped instead and reset state
                npcData.State = State.Connected;
 
                // Check if it was a stop caused by NPC death
                if(npcData.Health > 0.0 || npcData.IsDead == true)
                {
                    // Dont call this for death-stops
                    OnPlaybackStopped.Invoke(npc);
                    return true;
                }
 
                // Mark as finally dead in include
                npcData.IsDead = true;
 
                // Tell NPC he's dead so he wont execute playbacks anymore
                SendMessage(npc, -1, "RNPC:112");
 
                // 
                if(OnDeath.Invoke(npc, npcData.LastAssailant, npcData.DamageReason) == false)
                {
                    return true;
                }
 
                // Default behaviour
                if(npcData.DamageReason != Weapon.Vehicle)
                {
                    // If killed by vehicle, animation is already played
                    switch(new Random().Next(0, 4))
                    {
                        case 0: npc.ApplyAnimation("PED", "KO_skid_back", 4.1f, false, true, true, true, 0, true); break;
                        case 1: npc.ApplyAnimation("PED", "KO_skid_front", 4.1f, false, true, true, true, 0, true); break;
                        case 2: npc.ApplyAnimation("PED", "KO_spin_L", 4.1f, false, true, true, true, 0, true); break;
                        case 3: npc.ApplyAnimation("PED", "KO_spin_R", 4.1f, false, true, true, true, 0, true); break;
                    }
                }
 
                // CallLocalFunction("OnPlayerDeath", "iii", playerid, rnpcData[playerid][RNPC_LAST_DAMAGER], rnpcData[playerid][RNPC_LAST_REASON]);
 
                Timer respawnNPC = new Timer(TimeSpan.FromSeconds(4), false);
                respawnNPC.Tick += (sender, e) =>
                {
                    npc.Spawn();
                    respawnNPC.Dispose();
                };
                return true;
            }
 
            // NPC stopped playback (manual abort)
            if(command == "RNPC:003")
            {
                // Reset onfoot movements after manual playback stop
                if (npc.InAnyVehicle == false)
                {
                    return true;
                }
                
                // Create "blank" build and restore old static attributes
                RNPCInternal.Instance.CreateBuild(npc.Id, (int)RecordingType.OnFoot, 95);
                RNPCInternal.Instance.SetWeaponID((int)npc.Weapon);
                RNPCInternal.Instance.SetSpecialAction((int)npc.SpecialAction);
                RNPCInternal.Instance.SetAngleQuats(0.0f, 360.0f - npc.Angle, 0.0f);
                RNPCInternal.Instance.AddPause(5);
                RNPCInternal.Instance.FinishBuild();
 
                // OnRNPCPlaybackStopped is called when the stop-playback is finished
                npcData.State = State.Stopping;
 
                StartBuildPlayback(npc, 95);
                return true;
            }
 
            // NPC reports possible run-over vehicle
            // Do exact check and handle run-over
            if(command == "RNPC:301")
            {
                /*
                BaseVehicle vehicle = npc.Vehicle;
                BasePlayer driver = npc.Vehicle.Driver;
 
                if(IsVehicleOnPlayer(npc, vehicle) == false)
                {
                    return true;
                }
 
#if !defined RNPC_DONT_PAUSE_ON_VEHICLEHIT
                // On the first hit, pause playback         
                if (rnpcData[playerid][RNPC_VEHICLEHIT] == 0)
                {
                    RNPC_PauseRecordingPlayback(playerid);
                }
                // Reset the reset timer
                if (rnpcData[playerid][RNPC_VEHICLEHIT_RESETTIMER] > 0)
                {
                    KillTimer(rnpcData[playerid][RNPC_VEHICLEHIT_RESETTIMER]);
                }
                // Resets the vehicle hit status, even if the vehicle already is out of range           
                rnpcData[playerid][RNPC_VEHICLEHIT_RESETTIMER] = SetTimerEx("ResetVehicleHit", 2200, 0, "i", playerid);
#endif
 
                    if (!CallLocalFunction("OnRNPCVehicleHit", "iiii", playerid, driverid, vehicleid, rnpcData[playerid][RNPC_VEHICLEHIT]))
                    {
                        // Default behaviour
                        if (!rnpcData[playerid][RNPC_VEHICLEHIT])
                        {
                            // Initial hit damage
                            new Float:vx, Float: vy, Float: vz;
                            GetVehicleVelocity(vehicleid, vx, vy, vz);
                            SetRNPCHealth(playerid, GetRNPCHealth(playerid) - floatsqroot(vx * vx + vy * vy + vz * vz) * RNPC_VEHICLE_DMG_MOD, driverid, 49);
                        }
                        else
                        {
                            // Constant standing damage
                            SetRNPCHealth(playerid, GetRNPCHealth(playerid) - 1.0, driverid, 49);
                        }
 
                    }
                    rnpcData[playerid][RNPC_VEHICLEHIT]++;
                } /*else {
            // If vehicle just left the player, resume reset hitstate instantly
            // also setting up the stand-up resume timer
            if (rnpcData[playerid][RNPC_VEHICLEHIT] > 0) {
                // Reset the reset timer
                if ((rnpcData[playerid][RNPC_VEHICLEHIT_RESETTIMER] > 0) && 0) {
                    KillTimer(rnpcData[playerid][RNPC_VEHICLEHIT_RESETTIMER]);
                }   
                //ResetVehicleHit(playerid, RNPC_RESETVH_RESET);        
            }
        }
                */
                return true;
            }
            return true;
        }
 
        #endregion // "Callbacks"
    }
 
    public class RNPCInternal : NativeObjectSingleton<RNPCInternal>
    {
        // @since 0.2
        [NativeMethod(Function = "RNPC_CreateBuild")]
        public virtual int CreateBuild(int npcId, int type, int slot = 0)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_FinishBuild")]
        public virtual int FinishBuild(int clear = 1)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_AddMovement")]
        public virtual int AddMovement(float startX, float startY, float startZ, float endX, float endY, float endZ, float speed = (0.0057f), int useZMap = 0)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_ConcatMovement")]
        public virtual int ConcatMovement(float x, float y, float z, float speed = (0.0057f), int useZMap = 0)
        {
            throw new NativeNotImplementedException();
        }
        
        [NativeMethod(Function = "RNPC_AddPause")]
        public virtual int AddPause(int time, int comp = 0)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetUpdateRate")]
        public virtual int SetUpdateRate(int rate)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetLRKeys")]
        public virtual int SetLRKeys(int leftRight)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetUDKeys")]
        public virtual int SetUDKeys(int upDown)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetKeys")]
        public virtual int SetKeys(int keys)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetQuat1")]
        public virtual int SetQuat1(float w)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetQuat2")]
        public virtual int SetQuat2(float x)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetQuat3")]
        public virtual int SetQuat3(float y)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetQuat4")]
        public virtual int SetQuat4(float z)
        {
            throw new NativeNotImplementedException();
        }
 
 
        [NativeMethod(Function = "RNPC_SetHealth")]
        public virtual int SetHealth(int health)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetArmour")]
        public virtual int SetArmour(int armour)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetSpecialAction")]
        public virtual int SetSpecialAction(int specialAction)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetWeaponID")]
        public virtual int SetWeaponID(int weaponId)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetAnimID")]
        public virtual int SetAnimID(int animationId)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetAnimParams")]
        public virtual int SetAnimParams(int parameters)
        {
            throw new NativeNotImplementedException();
        }
        
        [NativeMethod(Function = "RNPC_SetAngleQuats")]
        public virtual int SetAngleQuats(float a, float h, float b)
        {
            throw new NativeNotImplementedException();
        }
 
        // @since 0.2.1
        [NativeMethod(Function = "RNPC_GetBuildLength")]
        public virtual int GetBuildLength()
        {
            throw new NativeNotImplementedException();
        }
 
        // @since 0.3
        [NativeMethod(Function = "RNPC_SetSirenState")]
        public virtual int SetSirenState(int state)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetDriverHealth")]
        public virtual int SetDriverHealth(int health)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetInternalPos")]
        public virtual int SetInternalPos(float x, float y, float z)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetAcceleration")]
        public virtual int SetAcceleration(float acceleration)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_SetDeceleration")]
        public virtual int SetDeceleration(float deceleration)
        {
            throw new NativeNotImplementedException();
        }
 
        [NativeMethod(Function = "RNPC_AddMovementAlt")]
        public virtual int AddMovementAlt(float startX, float startY, float startZ, float endX, float endY, float endZ, float speed = (0.0057f), bool locked = true)
        {
            throw new NativeNotImplementedException();
        }
 
        // @since 0.3.4
        [NativeMethod(Function = "RNPC_CreateCustomBuild")]
        public virtual int CreateCustomBuild(int type, string name)
        {
            throw new NativeNotImplementedException();
        }
    }
}