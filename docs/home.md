---
title: Apache Mesos - Documentation Home
layout: documentation
---

# Documentation


## Mesos Fundamentals

* [Mesos Architecture](architecture.html) providing an overview of Mesos concepts.
* [Video and Slides of Mesos Presentations](presentations.html)


## Running Mesos

* [Getting Started](getting-started.html) for basic instructions on compiling and installing Mesos.
* [Agent Recovery](agent-recovery.html) for doing seamless agent upgrades and allowing executors to survive `mesos-agent` crashes.
* [Authentication](authentication.html)
* [Authorization](authorization.html)
* [Configuration](configuration.html) and [CMake configuration](configuration-cmake.md) for command-line arguments.
* [Container Image](container-image.html) for supporting container images in Mesos containerizer.
* [Containerizers](containerizers.html) for containerizer overview and use cases.
  * [Containerizer Internals](containerizer-internals.html) for implementation details of containerizers.
  * [Docker Containerizer](docker-containerizer.html) for launching a Docker image as a Task, or as an Executor.
  * [Mesos Containerizer](mesos-containerizer.html) default containerizer, supports both Linux and POSIX systems.
    * [CNI support](cni.html)
    * [Docker Volume Support](docker-volume.html)
* [Framework Rate Limiting](framework-rate-limiting.html)
* [Task Health Checking](health-checks.html)
* [High Availability](high-availability.html) for running multiple masters simultaneously.
* [HTTP Endpoints](endpoints/) for available HTTP endpoints.
* [Logging](logging.html)
* [Maintenance](maintenance.html) for performing maintenance on a Mesos cluster.
* [Monitoring](monitoring.html)
* [Operational Guide](operational-guide.html)
* [Roles](roles.html)
* [Secrets](secrets.html) for managing secrets within Mesos.
* [SSL](ssl.html) for enabling and enforcing SSL communication.
* [Nested Container and Task Group (Pod)](nested-container-and-task-group.html)
* [Tools](tools.html) for setting up and running a Mesos cluster.
* [Upgrades](upgrades.html) for upgrading a Mesos cluster.
* [Weights](weights.html)
* [Windows Support](windows.html) for the state of Windows support in Mesos.


## Advanced Features

* [Attributes and Resources](attributes-resources.html) for how to describe the agents that comprise a cluster.
* [Fetcher Cache](fetcher.html) for how to configure the Mesos fetcher cache.
* [Multiple Disks](multiple-disk.html) for how to allow tasks to use multiple isolated disk resources.
* [Networking](networking.html)
  * [Container Network Interface (CNI)](cni.html)
  * [Port Mapping Isolator](port-mapping-isolator.html)
* [Nvidia GPU Support](gpu-support.html) for how to run Mesos with Nvidia GPU support.
* [Oversubscription](oversubscription.html) for how to configure Mesos to take advantage of unused resources to launch "best-effort" tasks.
* [Persistent Volume](persistent-volume.html) for how to allow tasks to access persistent storage resources.
* [Quota](quota.html) for how to configure Mesos to provide guaranteed resource allocations for use by a role.
* [Replicated Log](replicated-log-internals.html) for information on the Mesos replicated log.
* [Reservation](reservation.html) for how operators and frameworks can reserve resources on individual agents for use by a role.
* [Shared Resources](shared-resources.html) for how to share persistent volumes between tasks managed by different executors on the same agent.


## APIs
* [API Client Libraries](api-client-libraries.html) lists client libraries for the HTTP APIs.
* [Doxygen](/api/latest/c++/namespacemesos.html) documents the C++ API.
* [Executor HTTP API](executor-http-api.html) describes the new HTTP API for communication between executors and the Mesos agent.
* [Javadoc](/api/latest/java/) documents the old Java API.
* [Operator HTTP API](operator-http-api.html) describes the new HTTP API for communication between operators and Mesos master/agent.
* [Scheduler HTTP API](scheduler-http-api.html) describes the new HTTP API for communication between schedulers and the Mesos master.
* [Task State Reasons](task-state-reasons.html) describes how task state reasons are used in Mesos.
* [Versioning](versioning.html) describes HTTP API and release versioning.


## Running Mesos Frameworks

* [Mesos frameworks](frameworks.html) for a list of apps built on top of Mesos and instructions on how to run them.
* [Sandbox](sandbox.html) describes a useful debugging arena for most users.


## Developing Mesos Frameworks

* [Designing Highly Available Mesos Frameworks](high-availability-framework-guide.html)
* [Developer Tools](tools.html) for hacking on Mesos or writing frameworks.
* [Framework Development Guide](app-framework-development-guide.html) describes how to build applications on top of Mesos.
* [Reconciliation](reconciliation.html) for ensuring a framework's state remains eventually consistent in the face of failures.


## Extending Mesos

* [Allocation Modules](allocation-module.html) for how to write custom resource allocators.
* [Mesos Modules](modules.html) for specifying Mesos modules for master, agent and tests.


## Contributing to Mesos

* [Committers and Maintainers](committers.html) a listing of project committers and component maintainers; useful when seeking feedback.
* [Committing](committing.html) guidelines for committing changes.
* [Development Roadmap](roadmap.html)
* [Documentation Guide](documentation-guide.html)
  * [C++ Style Guide](c++-style-guide.html)
  * [Doxygen Style Guide](doxygen-style-guide.html)
  * [Markdown Style Guide](markdown-style-guide.html)
* [Doxygen](/api/latest/c++/) documents the internal Mesos APIs.
* [Effective Code Reviewing](effective-code-reviewing.html) guidelines, tips, and learnings for how to do effective code reviews.
* [Engineering Principles and Practices](engineering-principles-and-practices.html) to serve as a shared set of project-level values for the community.
* [Release Guide](release-guide.html)
* [Reopening a Review](reopening-reviews.html) for our policy around reviving reviews on ReviewBoard.
* [Reporting an Issue, Improvement, or Feature](reporting-a-bug.html) for getting started with JIRA.
* [Submitting a Patch](submitting-a-patch.html) for getting started with ReviewBoard and our tooling around it.
* [Testing Patterns](testing-patterns.html) for tips and tricks used in Mesos tests.
* [Working groups](working-groups.html) a listing of groups working on different components.


## More Info about Mesos

* [Academic Papers and Project History](https://www.usenix.org/conference/nsdi11/mesos-platform-fine-grained-resource-sharing-data-center)
* [Design docs](design-docs.html) list of design documents for various Mesos features
* [Powered by Mesos](powered-by-mesos.html) lists organizations and software that are powered by Apache Mesos.


## Books on Mesos

<div class="row">
  <div class="col-xs-6 col-md-4">
    <a href="https://www.packtpub.com/big-data-and-business-intelligence/apache-mesos-essentials" class="thumbnail">
      <img src="https://www.packtpub.com/sites/default/files/9781783288762.png" alt="Apache Mesos Essentials by Dharmesh Kakadia">
    </a>
    <p class="text-center">Apache Mesos Essentials by Dharmesh Kakadia (Packt, 2015)</p>
  </div>
  <div class="col-xs-6 col-md-4">
    <a href="http://shop.oreilly.com/product/0636920039952.do" class="thumbnail">
      <img src="http://akamaicovers.oreilly.com/images/0636920039952/lrg.jpg" alt="Building Applications on Mesos by David Greenberg">
    </a>
    <p class="text-center">Building Applications on Mesos by David Greenberg (O'Reilly, 2015)</p>
  </div>
  <div class="col-xs-6 col-md-4">
    <a href="https://www.packtpub.com/big-data-and-business-intelligence/mastering-mesos" class="thumbnail">
      <img src="https://www.packtpub.com/sites/default/files/6249OS_5186%20Mastering%20Mesos.jpg" alt="Mastering Mesos by Dipa Dubhashi and Akhil Das">
    </a>
    <p class="text-center">Mastering Mesos by Dipa Dubhashi and Akhil Das (Packt, 2016)</p>
  </div>
  <div class="col-xs-6 col-md-4">
    <a href="https://www.manning.com/books/mesos-in-action" class="thumbnail">
      <img src="https://images.manning.com/255/340/resize/book/d/62f5c9b-0946-4569-ad50-ffdb84876ddc/Ignazio-Mesos-HI.png" alt="Mesos in Action by Roger Ignazio">
    </a>
  <p class="text-center">Mesos in Action by Roger Ignazio (Manning, 2016)
  </div>
</div>
