#pragma once

class AppConstants final {
public:
    static constexpr int minGpibAddress() {
        return MinGpibAddress_;
    }

    static constexpr int maxGpibAddress() {
        return MaxGpibAddress_;
    }

    static constexpr int defaultGpibAddress() {
        return DefaultGpibAddress_;
    }

    static constexpr int defaultReadTimeoutMs() {
        return DefaultReadTimeoutMs_;
    }

    static constexpr int sidebarExpandedWidth() {
        return SidebarExpandedWidth_;
    }

    static constexpr int sidebarCollapsedWidth() {
        return SidebarCollapsedWidth_;
    }

private:
    AppConstants() = delete;

    static constexpr int MinGpibAddress_ = 1;
    static constexpr int MaxGpibAddress_ = 29;
    static constexpr int DefaultGpibAddress_ = 10;
    static constexpr int DefaultReadTimeoutMs_ = 5000;
    static constexpr int SidebarExpandedWidth_ = 390;
    static constexpr int SidebarCollapsedWidth_ = 96;
};
