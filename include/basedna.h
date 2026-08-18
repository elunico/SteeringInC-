#ifndef BASEDNA_H
#define BASEDNA_H

namespace tom {

template <typename Self>
struct BaseDNA {
    virtual ~BaseDNA() = default;
    BaseDNA() noexcept = default;

    [[nodiscard]]
    virtual Self crossover(Self const& partner) const noexcept = 0;
    virtual void mutate() noexcept                             = 0;
};

}  // namespace tom

#endif  // BASEDNA_H