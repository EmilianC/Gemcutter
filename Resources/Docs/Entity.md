# Example Queries
Any Component or Tag can be part of a query.
Only currently active Components and Entities are visible to queries.

```cpp
// Process all players by Entity.
for (Entity& e : With<Player>())
{
	//...
}

// Process all players by component.
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

// Process all friendly SquadLeaders on the local network.
for (Entity& e : With<Player, Friendly, LocalNetwork, SquadLeader>())
{
	//...
}
```