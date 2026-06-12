# Martingale

> A high-performance C++ framework for derivative valuation and quantitative risk analytics.

Disclaimer: This was not vibe coded. Every line and comment was written with intent and validated against closed-form analytical solutions, because a Monte Carlo engine that hasn't been sanity-checked against Black-Scholes is just an expensive random number generator.

---

## What is Martingale?

Martingale is a C++ derivatives pricing engine built from first principles. It simulates the evolution of asset prices under the **risk-neutral measure** $\mathbb{Q}$ using **Geometric Brownian Motion (GBM)** and discounts the expected payoff to obtain the present value of an option.

Under $\mathbb{Q}$, the asset price $S_t$ follows the stochastic differential equation:

$$dS_t = r\,S_t\,dt + \sigma\,S_t\,dW_t$$

where $r$ is the risk-free rate, $\sigma$ is the volatility, and $W_t$ is a standard Brownian motion.

The name comes from the mathematical concept of a **martingale process**, which is the foundation of risk-neutral pricing in quantitative finance. When the asset price is discounted by the risk-free money-market account, the resulting process is a martingale under $\mathbb{Q}$:

$$\mathbb{E}^{\mathbb{Q}}\!\left[\,e^{-r\,T}\,S_T \mid \mathcal{F}_t\,\right] = e^{-r\,t}\,S_t$$

This is precisely what guarantees an arbitrage-free price.

Integrating the GBM SDE exactly gives the terminal stock price under the risk-neutral measure:

$$S_T = S_0 \cdot \exp\!\left(\left(r - \tfrac{\sigma^2}{2}\right)T + \sigma\sqrt{T}\,Z\right), \qquad Z \sim \mathcal{N}(0,1)$$

The price of a European call with strike $K$ is the discounted expected payoff, estimated by averaging over $N$ simulated paths:

$$C \approx e^{-rT} \cdot \frac{1}{N} \sum_{i=1}^{N} \max\!\left(S_T^{(i)} - K,\; 0\right)$$

---

## Build & Run

### Prerequisites
- `clang++` or `g++` with C++20 support

### CLI
```bash
c++ -std=c++20 -O2 -Iinclude \
    src/Option.cpp src/MonteCarloPricer.cpp src/main.cpp \
    -o option_pricer && ./option_pricer
```

### Validation Test
```bash
c++ -std=c++20 -O2 -Iinclude \
    src/Option.cpp src/MonteCarloPricer.cpp tests/test_pricer.cpp \
    -o test_pricer && ./test_pricer
```

---

## License

MIT License © 2026 Gunj Joshi

You are free to use, modify, and distribute this software, provided that the original copyright notice and this permission notice are included in all copies or substantial portions of the software.
