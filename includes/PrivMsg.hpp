/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrivMsg.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luda-cun <luda-cun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 13:28:07 by wivallee          #+#    #+#             */
/*   Updated: 2026/08/23 14:52:42 by luda-cun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "Command.hpp"
#include "Channel.hpp"

class PrivMsg: public Command
{
private:
	std::vector<Client *> &clients_;
	std::vector<Channel *> &channels_;

public:
	PrivMsg(std::vector<Client *> &clients, std::vector<Channel *> &channels);
	~PrivMsg();

	void execute(Client &client, const std::string &args);

private:

	void message(Client &client, const std::string &msg);
};
