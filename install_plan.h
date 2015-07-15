// Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UPDATE_ENGINE_INSTALL_PLAN_H_
#define UPDATE_ENGINE_INSTALL_PLAN_H_

#include <string>
#include <vector>

#include <base/macros.h>
#include <chromeos/secure_blob.h>

#include "update_engine/action.h"

// InstallPlan is a simple struct that contains relevant info for many
// parts of the update system about the install that should happen.
namespace chromeos_update_engine {

struct InstallPlan {
  InstallPlan(bool is_resume,
              bool is_full_update,
              const std::string& url,
              uint64_t payload_size,
              const std::string& payload_hash,
              uint64_t metadata_size,
              const std::string& metadata_signature,
              const std::string& install_path,
              const std::string& kernel_install_path,
              const std::string& source_path,
              const std::string& kernel_source_path,
              const std::string& public_key_rsa);

  // Default constructor: Initialize all members which don't have a class
  // initializer.
  InstallPlan();

  bool operator==(const InstallPlan& that) const;
  bool operator!=(const InstallPlan& that) const;

  void Dump() const;

  bool is_resume;
  bool is_full_update;
  std::string download_url;  // url to download from
  std::string version;       // version we are installing.

  uint64_t payload_size;                 // size of the payload
  std::string payload_hash;             // SHA256 hash of the payload
  uint64_t metadata_size;                // size of the metadata
  std::string metadata_signature;        // signature of the  metadata
  std::string install_path;              // path to install device
  std::string kernel_install_path;       // path to kernel install device
  std::string source_path;               // path to source device
  std::string kernel_source_path;        // path to source kernel device

  // The fields below are used for kernel and rootfs verification. The flow is:
  //
  // 1. FilesystemVerifierAction computes and fills in the source partition
  // sizes and hashes.
  //
  // 2. DownloadAction verifies the source partition sizes and hashes against
  // the expected values transmitted in the update manifest. It fills in the
  // expected applied partition sizes and hashes based on the manifest.
  //
  // 3. FilesystemVerifierAction computes and verifies the applied and source
  // partition sizes and hashes against the expected values.
  uint64_t kernel_size;
  uint64_t rootfs_size;
  chromeos::Blob kernel_hash;
  chromeos::Blob rootfs_hash;
  chromeos::Blob source_kernel_hash;
  chromeos::Blob source_rootfs_hash;

  // True if payload hash checks are mandatory based on the system state and
  // the Omaha response.
  bool hash_checks_mandatory;

  // True if Powerwash is required on reboot after applying the payload.
  // False otherwise.
  bool powerwash_required;

  // If not blank, a base-64 encoded representation of the PEM-encoded
  // public key in the response.
  std::string public_key_rsa;
};

class InstallPlanAction;

template<>
class ActionTraits<InstallPlanAction> {
 public:
  // Takes the install plan as input
  typedef InstallPlan InputObjectType;
  // Passes the install plan as output
  typedef InstallPlan OutputObjectType;
};

// Basic action that only receives and sends Install Plans.
// Can be used to construct an Install Plan to send to any other Action that
// accept an InstallPlan.
class InstallPlanAction : public Action<InstallPlanAction> {
 public:
  InstallPlanAction() {}
  explicit InstallPlanAction(const InstallPlan& install_plan):
    install_plan_(install_plan) {}

  void PerformAction() override {
    if (HasOutputPipe()) {
      SetOutputObject(install_plan_);
    }
    processor_->ActionComplete(this, ErrorCode::kSuccess);
  }

  InstallPlan* install_plan() { return &install_plan_; }

  static std::string StaticType() { return "InstallPlanAction"; }
  std::string Type() const override { return StaticType(); }

  typedef ActionTraits<InstallPlanAction>::InputObjectType InputObjectType;
  typedef ActionTraits<InstallPlanAction>::OutputObjectType OutputObjectType;

 private:
  InstallPlan install_plan_;

  DISALLOW_COPY_AND_ASSIGN(InstallPlanAction);
};

}  // namespace chromeos_update_engine

#endif  // UPDATE_ENGINE_INSTALL_PLAN_H_