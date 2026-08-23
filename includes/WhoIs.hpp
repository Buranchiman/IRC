/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WhoIs.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luda-cun <luda-cun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:04:19 by buranchiman       #+#    #+#             */
/*   Updated: 2026/08/23 14:54:09 by luda-cun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "Command.hpp"
#include "Channel.hpp"

class WhoIs: public Command
{
private:
	std::vector<Client *> &clients_;
	std::vector<Channel *> &channels_;

public:
	WhoIs(std::vector<Client *> &clients, std::vector<Channel *> &channels);
	~WhoIs();

	void execute(Client &client, const std::string &args);

private:

};
