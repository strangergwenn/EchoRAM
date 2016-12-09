 # EchoRam API

EchoRam works with Json packets over TCP. 

## Connection

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

## Disconnection

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

## Updates

Updates are data commands writing new information about the player. This should be the entire player state, as it will overwrite the existing data entirely. You can send any JSON formatted data here.

```
{
	"update" :
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

## Heartbeats

Clients should send heartbeats regularly to ensure the data stays in the database, as it is garbage-collected periodically. The default server setting is 30s, so sending heartbeats every 10s is probably a good idea. Updates also work as heartbeats.

```
{
	"heartbeat" :
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

## Query

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

## Player search

Search for connected clients. You can specifiy multiple search criteria. Every criterion is made of a key to check, a value to compare with and a condition to use. The requesting client can be returned as part of the results.

```
{
	"search" :
	[
		{
			"key" : "level",
			"value" : 2,
			"condition" : ">="
		},
		{
			"key" : "level",
			"value" : 10,
			"condition" : "<"
		}
	]
}
```

The server reply will be sent as follow, as a map of clients.

```
{
	"reply" :
	{
        "status" : "OK",
        "clients" :
        {
			"<public-identifier>" : 
			{
				"name" : "Foobar",
				"level" : "2"
				// etc
			}
			,

			// etc
        }
	}
}
```

## Server stats

Get stats on the server.

```
{
	"stats" : 1
}
```

The server reply will be sent as follow, as a map of statistics.

```
{
	"reply" :
	{
		"status" : "OK",
		"count" : 42;
		"uptime" : 1256
	}
}
```
