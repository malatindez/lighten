#include "../components/components.hpp"
#include <iostream>
#include "mal-packet-weaver/packet-dispatcher.hpp"
namespace lighten::packets
{
    constexpr mal_packet_weaver::PacketSubsystemID PacketSubsystemNetwork = 0x0001;

    MAL_PACKET_WEAVER_DECLARE_PACKET(PingPacket, PacketSubsystemNetwork, 0, 120.0f)
    MAL_PACKET_WEAVER_DECLARE_PACKET(PongPacket, PacketSubsystemNetwork, 1, 120.0f)
    MAL_PACKET_WEAVER_DECLARE_PACKET_WITH_PAYLOAD(MessagePacket, PacketSubsystemNetwork, 2, 120.0f, (std::string, message))
    MAL_PACKET_WEAVER_DECLARE_PACKET(ConnectPacket, PacketSubsystemNetwork, 3, 120.0f)
    MAL_PACKET_WEAVER_DECLARE_PACKET(DisconnectPacket, PacketSubsystemNetwork, 4, 120.0f)
    MAL_PACKET_WEAVER_DECLARE_PACKET_WITH_PAYLOAD(ConnectResponsePacket, PacketSubsystemNetwork, 5, 120.0f, (bool, accepted))
    MAL_PACKET_WEAVER_DECLARE_PACKET_WITH_PAYLOAD(DisconnectResponsePacket, PacketSubsystemNetwork, 6, 120.0f, (bool, accepted))

    constexpr mal_packet_weaver::PacketSubsystemID PacketSubsystemComponentSynchro = 0x0002;
    //    MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(GameObjectPacket, (components::GameObject), PacketSubsystemComponentSynchro, 0, 120.0f)
    //    MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(TransformPacket, (components::Transform), PacketSubsystemComponentSynchro, 1, 120.0f)
    //    MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(PointLightPacket, (components::PointLight), PacketSubsystemComponentSynchro, 2, 120.0f)
    //    MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(DirectionalLightPacket, (components::DirectionalLight), PacketSubsystemComponentSynchro, 3, 120.0f)
    //    MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(SpotLightPacket, (components::SpotLight), PacketSubsystemComponentSynchro, 4, 120.0f)
    //    MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(ParticleEmitterPacket, (components::ParticleEmitter), PacketSubsystemComponentSynchro, 5, 120.0f)
    //    MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(ForceFieldPacket, (components::ForceField), PacketSubsystemComponentSynchro, 6, 120.0f)
    //    MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(ColliderPacket, (components::Collider), PacketSubsystemComponentSynchro, 7, 120.0f)
}