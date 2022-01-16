import numpy as np
import pandas as pd
from scipy.stats import wilcoxon
import matplotlib.pyplot as plt


def main():
    gd_results = pd.read_csv('../benchmark_results/greedy__default__benchmark.csv')
    pso_results = pd.read_csv('../benchmark_results/particle_swarm__default__benchmark.csv')
    # pso_results = pd.read_csv('../benchmark_results/particle_swarm__second__benchmark.csv')
    instances = gd_results['Instance ID'].unique()
    assert list(pso_results['Instance ID'].unique()) == list(instances)
    alg_keys = ['gd', 'pso']
    results = {'gd': gd_results, 'pso': pso_results}

    for alg_key in alg_keys:
        results[alg_key]['error'] = (results[alg_key]['Known optimum'] - results[alg_key]['Best algorithm solution']) / \
                                    results[alg_key]['Known optimum'] * 100

    test_results = pd.DataFrame(
        columns=['Instance ID', 'stat', 'p_val', 'gd_win', 'pso_win', 'gd_median', 'pso_median', 'gd_mean', 'gd_std',
                 'pso_mean', 'pso_std'])
    alpha = 0.05

    row = {}
    for iid in instances:
        row['Instance ID'] = iid
        results_gd_iid = results['gd'][results['gd']['Instance ID'] == iid]
        results_pso_iid = results['pso'][results['pso']['Instance ID'] == iid]
        row['gd_median'] = results_gd_iid['error'].median()
        row['pso_median'] = results_pso_iid['error'].median()
        row['gd_mean'] = results_gd_iid['error'].mean()
        row['gd_std'] = results_gd_iid['error'].std()
        row['pso_mean'] = results_pso_iid['error'].mean()
        row['pso_std'] = results_pso_iid['error'].std()

        row['gd_win'] = 0
        row['pso_win'] = 0
        try:
            stat, p_val = wilcoxon(results_gd_iid['error'].tolist(), results_pso_iid['error'].tolist())
            wilcoxon_success = True
        except ValueError as err:
            stat, p_val = -1, -1
            wilcoxon_success = False
        row['stat'] = stat
        row['p_val'] = p_val
        if wilcoxon_success and p_val < alpha:
            if results_gd_iid['error'].median() > results_pso_iid['error'].median():
                row['pso_win'] = 1
            else:
                row['gd_win'] = 1

        test_results = test_results.append(row, ignore_index=True)
        row = {}

    for c_name in ['Instance ID', 'gd_win', 'pso_win']:
        test_results[c_name] = test_results[c_name].astype(int)

    with pd.option_context('display.max_rows', None, 'display.max_columns', None):
        print(test_results)

    test_results.to_csv('test_results.csv', float_format='{0:.1e}'.format)

    wins = test_results[['gd_win', 'pso_win']].values
    gd_wins = np.count_nonzero(wins[:, 0])
    pso_wins = np.count_nonzero(wins[:, 1])
    draws = np.count_nonzero(wins[:, 0] == wins[:, 1])
    plt.bar([1, 2, 3], height=[gd_wins, draws, pso_wins], tick_label=['Wygrane Greedy', 'Remisy', 'Wygrane PSO'])
    plt.ylabel('Liczba instancji testowych')
    plt.title('Wyniki testu Wilcoxona dla wszystkich instancji testowych')
    plt.savefig('wilcoxon_bar')
    plt.close()


if __name__ == '__main__':
    main()
