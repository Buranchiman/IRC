# ft_irc

> An Internet Relay Chat (IRC) server implementation — a 42 School project.

Created as part of the **42 curriculum** by [luda-cun](https://github.com/Lucien-Da-Cunha) and [wivallee](https://github.com/Buranchiman).

---

## Table of Contents

- [Description](#description)
- [Getting Started](#getting-started)
  - [Build](#build)
  - [Usage](#usage)
- [What We Learned](#what-we-learned)
- [Command](#command)
- [Resources](#resources)
- [A Note on AI Usage](#a-note-on-ai-usage)
- [Authors](#authors)

---

## Description

This project aims to create an **Internet Relay Chat (IRC) server**.

IRC is a text-based communication protocol that allows users to talk to one another over the internet — in real time, in groups (via **channels**), or **privately**. Like most IRC servers, ours provides moderation commands to manage channels, user permissions, and channel access.

Rather than typing raw protocol-formatted messages, users typically connect through an **IRC client** for convenience. Our server was built with reference to [Halloy](https://github.com/squidowl/halloy), an open-source IRC client written in Rust using the Iced GUI library.

Through this project, we learned:
- How to use **sockets** to exchange data between multiple machines, terminals, and IRC client windows.
- How to use **poll()** to monitor multiple sockets for incoming data and keep the server running smoothly, without blocking.

---

## Getting Started

### Build

Compile the project from the root of the repository with a simple:

```sh
make
```

### Usage

The server takes two arguments: a **listening port** and a **connection password**.

```sh
./ircserv <port> <password>
```

**Example:**

```sh
./ircserv 6667 mypassword
```

---
## Command

```sh
.JOIN <channels>* [<key>]*
```
```sh
.MODE <channels>* [<argument>]*
```
```sh
.TOPIC <channels>* [<topic>]*
```
```sh
.INVITE <nickname> <channel>
```
```sh
.KICK <channel> <users>* [<comment>]
```

---
## Resources

| RFC | Description |
|---|---|
| [RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459) | The original, foundational standard defining basic IRC client-server and server-server communication. |
| [RFC 2810](https://datatracker.ietf.org/doc/html/rfc2810) | Overview of the IRC network architecture, entities, and basic functions. |
| [RFC 2811](https://datatracker.ietf.org/doc/html/rfc2811) | Rules and mechanics for creating and managing chat channels. |
| [RFC 2812](https://datatracker.ietf.org/doc/html/rfc2812) | Specific rules for client commands (e.g. `NICK`, `JOIN`, `PRIVMSG`). |

---

## A Note on AI Usage

AI was mainly used for **repetitive refactoring tasks** — for example, when changes to a class's members required corresponding updates to related functions across multiple files.

---

## Authors

- **[luda-cun](https://github.com/Lucien-Da-Cunha)**
- **[wivallee](https://github.com/Buranchiman)**
