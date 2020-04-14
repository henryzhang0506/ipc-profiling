#pragma once

#include <gflags/gflags.h>

#include <ipc/flags.h>
#include <metrics/latency_aggregator_reporter.h>
#include <metrics/registry.h>

#include "../common.h"

static void SetupMetrics() {
    using namespace drive::common::metrics;
    auto aggregator = std::make_shared<LatencyAggregatorReporter>();
    aggregator->initialize();
    aggregator->start();
    Registry::GetInstance()->add_reporter("latency_aggregator", std::move(aggregator));
}

static void SetupShmFlags() {
  FLAGS_ipc_pubsub_report_internal_metrics = true;
  FLAGS_ipc_pubsub_publisher_modes = "shm";
  FLAGS_ipc_pubsub_publisher_force_outgoing_queue_flush = true;
  FLAGS_ipc_pubsub_subscriber_ros_connection_management_mode = "off";

  FLAGS_ipc_pubsub_subscriber_modes = "shm";
  FLAGS_ipc_pubsub_subscriber_protocol = "tcp";
  FLAGS_ipc_pubsub_subscriber_tcp_nodelay = "true";
}

