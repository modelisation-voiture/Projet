#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <boost/numeric/odeint.hpp>

namespace Simulation {
namespace Solver {

// Classe template générique pour résoudre différents types d'EDO
template<typename State, typename Time = double>
class OdeSolver {
public:
    using StateType = State;
    using TimeType = Time;
    using SystemFunction = std::function<void(const State&, State&, Time)>;
    using ObserverFunction = std::function<void(const State&, Time)>;
    
    enum class Method {
        RK4,            // Runge-Kutta 4 (fixed step)
        RK45,           // Runge-Kutta-Fehlberg 45 (adaptive step)
        DormandPrince,  // Dormand-Prince 5(4) (adaptive step, meilleure précision)
        BulirschStoer   // Bulirsch-Stoer (systèmes raides)
    };
    
    OdeSolver(Method method = Method::DormandPrince) : m_method(method) {}
    
    // Intègre avec pas fixe et observe à chaque pas
    void integrate_const(
        const SystemFunction& system,
        State& state,
        Time start_time,
        Time end_time,
        Time step_size,
        const ObserverFunction& observer = nullptr);
    
    // Intègre avec pas adaptatif et observe à chaque pas adapté
    void integrate_adaptive(
        const SystemFunction& system,
        State& state,
        Time start_time,
        Time end_time,
        Time initial_step_size,
        const ObserverFunction& observer = nullptr);
    
    // Configure les tolérances pour l'intégration adaptative
    void set_tolerances(double abs_tolerance, double rel_tolerance) {
        m_abs_tolerance = abs_tolerance;
        m_rel_tolerance = rel_tolerance;
    }
    
    // Intègre et retourne un vecteur d'états à des pas réguliers
    std::vector<State> solve(
        const SystemFunction& system,
        const State& initial_state,
        Time start_time,
        Time end_time,
        Time output_step_size);

private:
    Method m_method;
    double m_abs_tolerance = 1.0e-6;
    double m_rel_tolerance = 1.0e-6;
    
    // Crée un stepper approprié selon la méthode choisie
    template<typename Stepper>
    Stepper create_stepper();
};

// Implémentation des méthodes

template<typename State, typename Time>
void OdeSolver<State, Time>::integrate_const(
    const SystemFunction& system,
    State& state,
    Time start_time,
    Time end_time,
    Time step_size,
    const ObserverFunction& observer) {
    
    using namespace boost::numeric::odeint;
    
    auto wrapped_system = [&system](const State& x, State& dxdt, Time t) {
        system(x, dxdt, t);
    };
    
    auto wrapped_observer = observer 
        ? [&observer](const State& x, Time t) { observer(x, t); }
        : [](const State&, Time) { };
    
    switch (m_method) {
        case Method::RK4:
            integrate_const(runge_kutta4<State>(), wrapped_system, state, 
                            start_time, end_time, step_size, wrapped_observer);
            break;
        case Method::RK45:
            integrate_const(runge_kutta_fehlberg78<State>(), wrapped_system, state, 
                            start_time, end_time, step_size, wrapped_observer);
            break;
        case Method::DormandPrince:
            integrate_const(runge_kutta_dopri5<State>(), wrapped_system, state, 
                            start_time, end_time, step_size, wrapped_observer);
            break;
        case Method::BulirschStoer:
            integrate_const(bulirsch_stoer<State>(), wrapped_system, state, 
                            start_time, end_time, step_size, wrapped_observer);
            break;
    }
}

template<typename State, typename Time>
void OdeSolver<State, Time>::integrate_adaptive(
    const SystemFunction& system,
    State& state,
    Time start_time,
    Time end_time,
    Time initial_step_size,
    const ObserverFunction& observer) {
    
    using namespace boost::numeric::odeint;
    
    auto wrapped_system = [&system](const State& x, State& dxdt, Time t) {
        system(x, dxdt, t);
    };
    
    auto wrapped_observer = observer 
        ? [&observer](const State& x, Time t) { observer(x, t); }
        : [](const State&, Time) { };
    
    switch (m_method) {
        case Method::RK4: {
            auto stepper = make_controlled(m_abs_tolerance, m_rel_tolerance, runge_kutta4<State>());
            integrate_adaptive(stepper, wrapped_system, state, 
                               start_time, end_time, initial_step_size, wrapped_observer);
            break;
        }
        case Method::RK45: {
            auto stepper = make_controlled(m_abs_tolerance, m_rel_tolerance, runge_kutta_fehlberg78<State>());
            integrate_adaptive(stepper, wrapped_system, state, 
                               start_time, end_time, initial_step_size, wrapped_observer);
            break;
        }
        case Method::DormandPrince: {
            auto stepper = make_controlled(m_abs_tolerance, m_rel_tolerance, runge_kutta_dopri5<State>());
            integrate_adaptive(stepper, wrapped_system, state, 
                               start_time, end_time, initial_step_size, wrapped_observer);
            break;
        }
        case Method::BulirschStoer: {
            auto stepper = make_controlled(m_abs_tolerance, m_rel_tolerance, bulirsch_stoer<State>());
            integrate_adaptive(stepper, wrapped_system, state, 
                               start_time, end_time, initial_step_size, wrapped_observer);
            break;
        }
    }
}

template<typename State, typename Time>
std::vector<State> OdeSolver<State, Time>::solve(
    const SystemFunction& system,
    const State& initial_state,
    Time start_time,
    Time end_time,
    Time output_step_size) {
    
    std::vector<State> results;
    State state = initial_state;
    
    // Observer qui enregistre les résultats à chaque pas
    auto observer = [&results](const State& x, Time) {
        results.push_back(x);
    };
    
    // Intégration adaptative avec un pas initial
    integrate_adaptive(system, state, start_time, end_time, output_step_size, observer);
    
    return results;
}

} // namespace Solver
} // namespace Simulation
