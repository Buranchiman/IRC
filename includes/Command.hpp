/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include "Client.hpp"

class Channel;

/**
 * @brief Abstract base class for IRC commands
 * 
 * All IRC commands inherit from this class and implement the execute method
 */
class Command
{
public:
	virtual ~Command() {}
	
	/**
	 * @brief Execute the command
	 * 
	 * @param client The client executing the command
	 * @param args The arguments passed to the command
	 * @return void
	 */
	virtual void execute(Client &client, const std::string &args) = 0;
};

