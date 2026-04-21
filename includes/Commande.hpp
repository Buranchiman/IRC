/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commande.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luda-cun <luda-cun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 14:27:31 by luda-cun          #+#    #+#             */
/*   Updated: 2026/04/21 17:03:04 by luda-cun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Client.hpp"
#include "Channel.hpp"
class Commande
{
private:
	/* data */
public:
	Commande(std::vector<Client> client, Channel channel);
	~Commande();

	// Fonction de commande 
	void	join(Client &client, std::string const &channel_name, std::string const &key);
	void	kick(Client &client, Channel &channel, std::string const &target_name, std::string const &reason);
	void	invite(Client &client, Channel &channel, std::string const &target_name);
	void	topic(Client &client, Channel &channel, std::string const &new_topic);
	void	mode(Client &client, Channel &channel, std::string const &mode_str, std::string const &args);
};

Commande::Commande(std::vector<Client> client, Channel channel)
{
	(void)client;
	(void)channel;
}

Commande::~Commande()
{
}
