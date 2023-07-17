#pragma once

#include "filesystem.h"
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs
{
class syncer
{
public:
    using rename_pair_t = std::pair<fs::path, fs::path>;

    using on_entry_created_t = std::function<void(const fs::path&, const std::vector<fs::path>&, bool)>;
    using on_entry_modified_t = std::function<void(const fs::path&, const std::vector<fs::path>&, bool)>;
    using on_entry_removed_t = std::function<void(const fs::path&, const std::vector<fs::path>&)>;
    using on_entry_renamed_t = std::function<void(const rename_pair_t&, const std::vector<rename_pair_t>&)>;

    struct mapping
    {
        std::vector<std::string> extensions;
        on_entry_created_t on_entry_created;
        on_entry_modified_t on_entry_modified;
        on_entry_removed_t on_entry_removed;
        on_entry_renamed_t on_entry_renamed;
    };
    using mapping_t = std::unordered_map<std::string, mapping>;

    syncer() = default;
    ~syncer();

    //-----------------------------------------------------------------------------
    //  Name : set_mapping ()
    /// <summary>
    /// Remaps a specific extension of the reference directory
    /// to extensions of the synced directory. You will get notified with entries
    /// of the synced directory with 'synced_ext' when an entry with the ref_ext
    /// in the reference directory gets changed.
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_mapping(const std::string& ref_ext,
                     const std::vector<std::string>& synced_ext,
                     on_entry_created_t on_entry_created,
                     on_entry_modified_t on_entry_modified,
                     on_entry_removed_t on_entry_removed,
                     on_entry_renamed_t on_entry_renamed);

    void set_directory_mapping(on_entry_created_t on_entry_created,
                               on_entry_modified_t on_entry_modified,
                               on_entry_removed_t on_entry_removed,
                               on_entry_renamed_t on_entry_renamed);

    //-----------------------------------------------------------------------------
    //  Name : sync ()
    /// <summary>
    /// Start syncing the synced_dir with reference to the reference_dir i.e
    /// changes that occur in the reference_dir will be reported on entries in the
    /// synced_dir.
    /// </summary>
    //-----------------------------------------------------------------------------
    void sync(const fs::path& reference_dir, const fs::path& synced_dir);

    //-----------------------------------------------------------------------------
    //  Name : unsync ()
    /// <summary>
    /// Stops syncing.
    /// </summary>
    //-----------------------------------------------------------------------------
    void unsync();

private:
    auto get_mapping(const std::string& ext) -> mapping;
    auto get_on_created_callback(const std::string& ext) -> on_entry_created_t;
    auto get_on_modified_callback(const std::string& ext) -> on_entry_modified_t;
    auto get_on_removed_callback(const std::string& ext) -> on_entry_removed_t;
    auto get_on_renamed_callback(const std::string& ext) -> on_entry_renamed_t;
    //-----------------------------------------------------------------------------
    //  Name : get_synced_directory ()
    /// <summary>
    /// Given a path, return the coresponding directory in the synced directory
    /// </summary>
    //-----------------------------------------------------------------------------
    auto get_synced_directory(const fs::path& path) -> fs::path;

    //-----------------------------------------------------------------------------
    //  Name : get_path_protocols ()
    /// <summary>
    /// Given a path, return the coresponding entries in the synced directory
    /// </summary>
    //-----------------------------------------------------------------------------
    auto get_synced_entries(const fs::path& path, bool is_directory) -> std::vector<fs::path>;

    //-----------------------------------------------------------------------------
    //  Name : get_watch_path ()
    /// <summary>
    /// Returns the constructed path for the watcher.
    /// </summary>
    //-----------------------------------------------------------------------------
    auto get_watch_path() -> fs::path;

    /// Mutex for locking the data of this class.
    std::mutex mutex_;
    /// Mappings of all remapped extensions.
    mapping_t mapping_;
    /// Reference directory to be watched.
    fs::path reference_dir_;
    /// Directory to be synced with the reference one.
    fs::path synced_dir_;

    std::atomic<std::uint64_t> watch_id_ = {0};
};

//////////////////////////////////////////////////////////////////////
/// Usage:
///
/// //Whatever happens to original_dir you will get notified and suggested to take action for
/// //the associated mappings in the synced_dir. For example if a .txt file was deleted from
/// //orignal_dir you will get notified and will be listed the .txt and .meta files in the
/// //synced_dir. It is your decision what to do with that. Maybe you want to delete them too
/// //or take any action you think is appropriate.
///
/// auto on_removed = [](const auto& ref_path, const auto& synced_paths)
/// {
/// 	for(const auto& synced_path : synced_paths)
/// 	{
/// 		fs::error_code err;
/// 		fs::remove_all(synced_path, err);
/// 	}
/// };
///
/// fs::syncer syncer;
/// syncer.set_directory_mapping(on_dir_create, on_dir_modified, on_dir_removed, on_dir_renamed);
/// syncer.set_mapping(".txt", {".txt", ".meta"}, on_create, on_modified, on_removed, on_renamed);
/// syncer.sync(original_dir, synced_dir);
///
/// while(!should_exit)
/// {
///     ...
/// }
} // namespace fs
