/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TopicCommand.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luda-cun <luda-cun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 14:53:42 by luda-cun          #+#    #+#             */
/*   Updated: 2026/08/23 14:53:44 by luda-cun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once
#include <vector>
#include "Command.hpp"
#include "Channel.hpp"

class TopicCommand : public Command
{
private:
	std::vector<Channel *> &channels_;

public:
	TopicCommand(std::vector<Channel *> &channels);
	~TopicCommand();

	void execute(Client &client, const std::string &args);
	

private:
	void topic(Client &client, const std::string &new_topic);
	void topic(Client &client, const std::string &channel_name, const std::string &new_topic);
};
