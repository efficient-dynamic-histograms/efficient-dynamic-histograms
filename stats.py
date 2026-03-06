import numpy as np
from scipy.integrate import cumulative_trapezoid, trapezoid as trapz
from scipy.stats import beta, norm, uniform


pdfs = {
    'beta':lambda x: (beta.pdf(x, a=100, b=1) + beta.pdf(x, a=1, b=100))/2,
    'normal': lambda x: norm.pdf(x),
    'uni3': lambda x: np.sum([uniform.pdf(x, loc=a, scale=1) for a in [0,2,4]],axis=0)/3,
    'uni29':lambda x: np.sum([uniform.pdf(x, loc=a, scale=1) for a in range(0,30,2)],axis=0)/30,
    'pareto':lambda x: 1.0/x**2
}

def calc_dkl(name, x, est):
    return kl_divergence(x, pdfs[name](x), est)

def calc_cm(name, x, est):
    return cramer_von_mises(x, pdfs[name](x), est)

def calc_ks(name, x, est):
    return kolmogorov_smirnov(x, pdfs[name](x), est)

def calc_ad(name, x, est):
    return anderson_darling(x, pdfs[name](x), est)


def kl_divergence(x, p_vals, q_vals, eps=1e-4):
    p = np.asarray(p_vals, dtype=np.float64)
    q = np.asarray(q_vals, dtype=np.float64)
    dx = x[1] - x[0]

    p /= np.sum(p * dx)
    q /= np.sum(q * dx)

    safe_q = np.clip(q, eps, None)
    safe_p = np.clip(p, eps, None)

    integrand = p * np.log(safe_p / safe_q)
    kl = np.sum(integrand) * dx

    if kl < -1e-8:
        raise ValueError(f"KL divergence is strongly negative ({kl}) — likely a bug.")

    return kl


def cramer_von_mises(x, p_vals, q_vals):
    p = np.asarray(p_vals)
    q = np.asarray(q_vals)

    p = p / trapz(p, x)
    q = q / trapz(q, x)

    F = cumulative_trapezoid(p, x, initial=0)
    G = cumulative_trapezoid(q, x, initial=0)

    diff_squared = (F - G)**2
    f_star = p

    return trapz(f_star * diff_squared, x)

def kolmogorov_smirnov(x, p_vals, q_vals):
    p = np.asarray(p_vals)
    q = np.asarray(q_vals)

    p = p / trapz(p, x)
    q = q / trapz(q, x)

    F = cumulative_trapezoid(p, x, initial=0)
    G = cumulative_trapezoid(q, x, initial=0)

    return np.max(np.abs(F - G))


def anderson_darling(x, p_vals, q_vals, eps=1e-8):
    p = np.asarray(p_vals)
    q = np.asarray(q_vals)

    p = p / trapz(p, x)
    q = q / trapz(q, x)

    F = cumulative_trapezoid(p, x, initial=0)
    G = cumulative_trapezoid(q, x, initial=0)

    F = np.clip(F, eps, 1 - eps)
    G = np.clip(G, eps, 1 - eps)
    mask = (F != eps) & (F != 1-eps) & (G!=eps) & (G!=1-eps) 

    num = (F - G)**2
    denom = F * (1 - F)

    return trapz(num[mask] / denom[mask], x[mask])
