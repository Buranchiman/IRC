/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KickCommand.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Command.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class KickCommand : public Command
{
private:
    std::vector<Client*>  &clients_;
    std::vector<Channel>  &channels_;

public:
    KickCommand(std::vector<Client*> &clients, std::vector<Channel> &channels);
    ~KickCommand();

    void execute(Client &client, const std::string &args);

private:
    void kick(Client &client,
              const std::string &channel_name,
              const std::string &target_name,
              const std::string &reason);
};