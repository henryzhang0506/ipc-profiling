#pragma once

#include <gflags/gflags.h>

#include <common/ipc/shm_message.pb.h>
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

  FLAGS_ipc_pubsub_publisher_modes = []() {
      if (const char * env_val = getenv("IPC_PUBSUB_PUBLISHER_MODES")) {
          return env_val;
      } else {
          return "shm";
      }
  }();

  FLAGS_ipc_pubsub_publisher_use_shm_allocators = []() {
      if (const char * env_val = getenv("IPC_PUBSUB_PUBLISHER_USE_SHM_ALLOCATORS")) {
          return env_val == std::string("1") || env_val == std::string("true");
      } else {
          return true;
      }
  }();

  FLAGS_ipc_pubsub_publisher_force_outgoing_queue_flush = []() {
      if (const char * env_val = getenv("IPC_PUBSUB_PUBLISHER_FORCE_OUTGOING_QUEUE_FLUSH")) {
          return env_val == std::string("1") || env_val == std::string("true");
      } else {
          return true;
      }
  }();


  FLAGS_ipc_pubsub_subscriber_ros_connection_management_mode = "off";

  FLAGS_ipc_pubsub_subscriber_modes = []() {
      if (const char * env_val = getenv("IPC_PUBSUB_SUBSCRIBER_MODES")) {
          return env_val;
      } else {
          return "shm";
      }
  }();

  FLAGS_ipc_pubsub_subscriber_protocol = "tcp";
  FLAGS_ipc_pubsub_subscriber_tcp_nodelay = "true";
}

static drive::common::ipc::ShmMessage
GenerateProtoMessage(size_t payload_size) {
    const auto alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const auto alphabet_size = strlen(alphabet);
    drive::common::ipc::ShmMessage msg;
    auto& payload = *msg.mutable_payload();
    payload.resize(payload_size);
    for (auto& c : payload) {
        c = alphabet[rand() % alphabet_size];
    }
    return msg;
}
