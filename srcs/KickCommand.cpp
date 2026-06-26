// KickCommand.cpp
#include "../includes/KickCommand.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

KickCommand::KickCommand(std::vector<Client*> &clients, std::vector<Channel> &channels)
    : clients_(clients), channels_(channels)
{}

KickCommand::~KickCommand() {}

void KickCommand::execute(Client &client, const std::string &args)
{
    if (args.empty()) {
        std::string msg = "461 " + client.getNickName() + " KICK :Not enough parameters\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }

    std::istringstream ss(args);
    std::string channel_name, target_name, reason;

    ss >> channel_name >> target_name;
    std::getline(ss, reason);

    if (!reason.empty() && reason[0] == ' ')
        reason = reason.substr(1);
    if (!reason.empty() && reason[0] == ':')
        reason = reason.substr(1);
    if (reason.empty())
        reason = target_name;

    if (channel_name.empty() || target_name.empty()) {
        std::string msg = "461 " + client.getNickName() + " KICK :Not enough parameters\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }

    kick(client, channel_name, target_name, reason);
}

void KickCommand::kick(Client &client,
                       const std::string &channel_name,
                       const std::string &target_name,
                       const std::string &reason)
{
    // 1. Trouver le channel
    Channel *chan = NULL;
    for (size_t i = 0; i < channels_.size(); i++) {
        if (channels_[i].getName() == channel_name) {
            chan = &channels_[i];
            break;
        }
    }
    if (!chan) {
        std::string msg = "403 " + client.getNickName() + " "
                        + channel_name + " :No such channel\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }

    // 2. Le kicker doit être dans le channel
    // findClientByNickname retourne NULL si pas trouvé
    if (!chan->findClientByNickname(client.getNickName())) {
        std::string msg = "442 " + client.getNickName() + " "
                        + channel_name + " :You're not on that channel\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }

    // 3. Le kicker doit être opérateur
    if (!chan->isOperator(client)) {
        std::string msg = "482 " + client.getNickName() + " "
                        + channel_name + " :You're not channel operator\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }

    // 4. Trouver la cible dans le channel
    Client *target = chan->findClientByNickname(target_name);
    if (!target) {
        std::string msg = "441 " + client.getNickName() + " "
                        + target_name + " " + channel_name
                        + " :They aren't on that channel\r\n";
        send_all(client.getFdSocket(), msg);
        return;
    }

    // 5. Broadcast avant suppression
    std::string kick_msg = ":" + client.getNickName()
                         + "!" + client.getUserName()
                         + " KICK " + channel_name
                         + " " + target_name
                         + " :" + reason + "\r\n";
    chan->broadcastToAll(kick_msg);

    // 6. Supprimer la cible du channel des deux côtés
    chan->leave(*target);        // Channel::leave(Client&)
    target->suppChannel(chan);   // Client::suppChannel(Channel*)
}