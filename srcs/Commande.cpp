/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commande.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 18:30:00 by githubcopil       #+#    #+#             */
/*   Updated: 2026/05/05 01:56:02 by lucien           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Commande.hpp"

Commande::Commande(std::vector<Client> &clients, std::vector<Channel> &channels)
	: clients_(&clients), channels_(&channels)
{
}

Commande::~Commande()
{
}
