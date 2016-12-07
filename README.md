# EchoRam

EchoRam is a lightweight matchmaking service for connecting players together, without complex setup, database, or game-specific behaviour. **Not ready for any kind of use yet**.

Features : 

 * RAM-only database for string attributes (level name, player name, player skill, etc)
 * Attribute search (get player named foobar, etc)
 * Simple update & query system
 * Plain TCP sockets
 
TODO : 
 * Options parser for limits
 * Matchmaking
 * Garbage collection
 * SSL sockets

## Command structure

EchoRam works with Json packets over TCP. 

### Connection

Connecting to the database makes the player searchable. The client needs access to something private and unique - a hash of the game key, Steam identifier, etc. This can change from one session to another, but doing so prevents any kind of friend system, for example.

```
{
	"connect" :
	{
		"privateId" : "<identifier>"
	}
}
```

The server reply will be sent as follow.

```
{
	"reply" :
	{
		"publicId" : "<public-identifier>"
		"status" : "OK"
	}
}
```

The public identifier is what other players will use to query the database, for example to check their friend's only status.

### Disconnection

Disconnecting removes all data from the database. 

```
{
	"disconnect" :
	{
		"privateId" : "<identifier>"
	}
}
```

The server reply will be sent as follow.

```
{
	"reply" :
	{
		"status" : "OK"
	}
}
```

### Heartbeat

Heartbeats are data commands writing new information about the player. This should be the entire player state, as it will overwrite the existing data entirely. You can send any JSON formatted data here.

```
{
	"heartbeat" :
	{
		"privateId" : "<identifier>",
		"data" :
		{
			"name" : "Foobar",
			"level" : "2"
			// etc
		}
	}
}
```

The server reply will be sent as follow.

```
{
	"reply" :
	{
		"status" : "OK"
	}
}
```

### Query

Queries fetch the player data from the public identifier.

```
{
	"query" :
	{
		"targetId" : "<public-identifier>"
	}
}
```

The server reply will be sent as follow, mirroring the last heartbeat for this player.

```
{
	"reply" :
	{
		"status" : "OK",
		"data" : 
		{
			"name" : "Foobar",
			"level" : "2"
			// etc
		}
	}
}
```

### Search

Search for players by attribute key + value. 

```
{
	"search" :
	{
		"key" : "name",
		"value" : "Foobar"
	}
}
```

The server reply will be sent as follow, as a map of public identifier -> attribute value.

```
{
	"reply" :
	{
		"status" : "OK",
		"clients" : 
		{
			"<public-identifier>" : "Foobar",
		}
	}
}
```
