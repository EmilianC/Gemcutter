# Dynamic Queries
A query will only return active Components and Entities. Any Component or Tag can be part of a query.

Queries are implemented in a LINQ-style fashion. This means that they are very efficient (zero dynamic allocations),
but also that Components/Tags of the queried types should not be created or deleted during the loop.

# Examples
```cpp
class Player       : public Component<Player> { /**/ };
class Network      : public Component<Network> { /**/ };
class Friendly     : public Tag<Friendly> {};
class Enemy        : public Tag<Enemy> {};
class SquadLeader  : public Tag<SquadLeader> {};

// Process all Entities with a Player Component.
for (Entity& e : With<Player>())
{
	//...
}

// Using All<>() processes the Component directly.
// This avoids the cost of calling Entity.Get<>().
for (Player& p : All<Player>())
{
	//...
}

// Process all enemy players.
for (Entity& e : With<Player, Enemy>())
{
	//...
}

// Process all friendly players.
for (Entity& e : With<Player, Friendly>())
{
	//...
}

// Process all friendly SquadLeaders on the network.
for (Entity& e : With<Player, Friendly, Network, SquadLeader>())
{
	//...
}
```