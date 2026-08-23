/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Who.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: buranchiman <buranchiman@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:04:19 by buranchiman       #+#    #+#             */
/*   Updated: 2026/05/26 16:11:54 by buranchiman      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include "Command.hpp"
#include "Channel.hpp"

class Who: public Command
{
private:
	std::vector<Client *> &clients_;
	std::vector<Channel *> &channels_;

public:
	Who(std::vector<Client *> &clients, std::vector<Channel *> &channels);
	~Who();

	void execute(Client &client, const std::string &args);

private:

};
