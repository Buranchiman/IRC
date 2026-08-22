/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commande.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wivallee <wivallee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 18:30:00 by githubcopil       #+#    #+#             */
/*   Updated: 2026/08/22 18:09:54 by wivallee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Commande.hpp"

Commande::Commande(std::vector<Client *> &clients, std::vector<Channel *> &channels)
	: clients_(clients), channels_(channels)
{
}

