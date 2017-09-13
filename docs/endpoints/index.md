---
title: Apache Mesos - HTTP Endpoints
layout: documentation
---
<!--- This is an automatically generated file. DO NOT EDIT! --->

# HTTP Endpoints #

Below is a list of HTTP endpoints available for a given Mesos process.

Depending on your configuration, some subset of these endpoints will be
available on your Mesos master or agent. Additionally, a `/help`
endpoint will be available that displays help similar to what you see
below.

** NOTE: ** If you are using Mesos 1.1 or later, we recommend using the
new [v1 Operator HTTP API](../operator-http-api.html) instead of the
unversioned REST endpoints listed below. These endpoints will be
deprecated in the future.


** NOTE: ** The documentation for these endpoints is auto-generated from
the Mesos source code. See `support/generate-endpoint-help.py`.

## Master Endpoints ##

Below are the endpoints that are available on a Mesos master. These
endpoints are reachable at the address `http://ip:port/endpoint`.

For example, `http://master.com:5050/files/browse`.

### files ###
* [/files/browse](files/browse.html)
* [/files/browse.json](files/browse.json.html)
* [/files/debug](files/debug.html)
* [/files/debug.json](files/debug.json.html)
* [/files/download](files/download.html)
* [/files/download.json](files/download.json.html)
* [/files/read](files/read.html)
* [/files/read.json](files/read.json.html)

### logging ###
* [/logging/toggle](logging/toggle.html)

### master ###
* [/api/v1](master/api/v1.html)
* [/api/v1/scheduler](master/api/v1/scheduler.html)
* [/create-volumes](master/create-volumes.html)
* [/destroy-volumes](master/destroy-volumes.html)
* [/flags](master/flags.html)
* [/frameworks](master/frameworks.html)
* [/health](master/health.html)
* [/machine/down](master/machine/down.html)
* [/machine/up](master/machine/up.html)
* [/maintenance/schedule](master/maintenance/schedule.html)
* [/maintenance/status](master/maintenance/status.html)
* [/quota](master/quota.html)
* [/redirect](master/redirect.html)
* [/reserve](master/reserve.html)
* [/roles](master/roles.html)
* [/roles.json](master/roles.json.html)
* [/slaves](master/slaves.html)
* [/state](master/state.html)
* [/state-summary](master/state-summary.html)
* [/state.json](master/state.json.html)
* [/tasks](master/tasks.html)
* [/tasks.json](master/tasks.json.html)
* [/teardown](master/teardown.html)
* [/unreserve](master/unreserve.html)
* [/weights](master/weights.html)

### metrics ###
* [/metrics/snapshot](metrics/snapshot.html)

### profiler ###
* [/profiler/start](profiler/start.html)
* [/profiler/stop](profiler/stop.html)

### registrar(id) ###
* [/registrar(id)/registry](registrar/registry.html)

### system ###
* [/system/stats.json](system/stats.json.html)

### version ###
* [/version](version.html)

## Agent Endpoints ##

Below are the endpoints that are available on a Mesos agent. These
endpoints are reachable at the address `http://ip:port/endpoint`.

For example, `http://agent.com:5051/files/browse`.

### files ###
* [/files/browse](files/browse.html)
* [/files/browse.json](files/browse.json.html)
* [/files/debug](files/debug.html)
* [/files/debug.json](files/debug.json.html)
* [/files/download](files/download.html)
* [/files/download.json](files/download.json.html)
* [/files/read](files/read.html)
* [/files/read.json](files/read.json.html)

### logging ###
* [/logging/toggle](logging/toggle.html)

### metrics ###
* [/metrics/snapshot](metrics/snapshot.html)

### profiler ###
* [/profiler/start](profiler/start.html)
* [/profiler/stop](profiler/stop.html)

### slave(id) ###
* [/api/v1](slave/api/v1.html)
* [/api/v1/executor](slave/api/v1/executor.html)
* [/api/v1/resource_provider](slave/api/v1/resource_provider.html)
* [/containers](slave/containers.html)
* [/flags](slave/flags.html)
* [/health](slave/health.html)
* [/monitor/statistics](slave/monitor/statistics.html)
* [/monitor/statistics.json](slave/monitor/statistics.json.html)
* [/state](slave/state.html)
* [/state.json](slave/state.json.html)

### system ###
* [/system/stats.json](system/stats.json.html)

### version ###
* [/version](version.html)
