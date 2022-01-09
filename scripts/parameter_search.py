import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


def main():
    directory_str = '../benchmark_results'
    directory = os.fsencode(directory_str)

    p_names = ['iter', 'mv', 'ptc', 'vl_vg']
    p_names_plot = {'iter': 'Iteracje', 'mv': 'Maksymalna prędkość', 'ptc': 'Cząstki'}
    stats = ['mean_time', 'std_time', 'mean_err', 'std_err', 'p_val']
    tested_instances = [1, 2, 8, 12, 26, 38, 40, 43]
    data_points = {
        p_name: {stat: {instance_id: [] for instance_id in tested_instances} if stat != 'p_val' else [] for stat in
                 stats}
        for p_name in p_names}

    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        if 'default' in filename or 'placeholder' in filename:
            continue
        filename_split = filename.split(sep='__')
        p_name, p_value = filename_split[1].split(sep='_')
        p_value = float(p_value.replace('-', '.'))
        if p_name == 'vl':
            _, p_value2 = filename_split[2].split(sep='_')
            p_value2 = float(p_value2.replace('-', '.'))

        df = pd.read_csv(os.path.join(directory_str, filename))
        df['error'] = (df['Known optimum'] - df['Best algorithm solution']) / df['Known optimum'] * 100
        means_df = df.groupby('Instance ID').mean()
        std_df = df.groupby('Instance ID').std()
        if p_name == 'vl':
            p_name = 'vl_vg'
        for index, row in means_df.iterrows():
            data_points[p_name]['mean_time'][index].append(row['Time'])
            data_points[p_name]['mean_err'][index].append(row['error'])
        for index, row in std_df.iterrows():
            data_points[p_name]['std_time'][index].append(row['Time'])
            data_points[p_name]['std_err'][index].append(row['error'])
        if p_name == 'vl_vg':
            data_points[p_name]['p_val'].append((p_value, p_value2))
        else:
            data_points[p_name]['p_val'].append(p_value)

    for pn in ['iter', 'mv', 'ptc']:
        plot_instances(data_points[pn]['p_val'], data_points[pn]['mean_err'], data_points[pn]['std_err'],
                       tested_instances,
                       'Błąd względny [%]', p_names_plot[pn], f'err_{pn}')
        plot_instances(data_points[pn]['p_val'], data_points[pn]['mean_time'], data_points[pn]['std_time'],
                       tested_instances,
                       'Czas wykonania [ms]', p_names_plot[pn], f'time_{pn}')
        fig, axs = plt.subplots(1, 2, figsize=(10, 5))
        plot_instances_mean(axs[0], data_points[pn]['p_val'], data_points[pn]['mean_err'], tested_instances,
                            p_names_plot[pn], 'Błąd względny [%]')
        plot_instances_mean(axs[1], data_points[pn]['p_val'], data_points[pn]['mean_time'], tested_instances,
                            p_names_plot[pn], 'Czas wykonania [ms]')
        plt.tight_layout()
        plt.savefig(f'../figures/param_mean_{pn}')
        plt.close()

    pval_np = np.array(data_points['vl_vg']['p_val'])
    ylabels = np.unique(pval_np[:, 0])
    xlabels = np.unique(pval_np[:, 1])

    iter_num = 0
    for yl in xlabels:
        for xl in ylabels:
            assert data_points['vl_vg']['p_val'][iter_num][0] == yl
            assert data_points['vl_vg']['p_val'][iter_num][1] == xl
            assert pval_np[iter_num, 0] == yl
            assert pval_np[iter_num, 1] == xl
            iter_num += 1

    plot_heatmap(xlabels, ylabels, data_points['vl_vg']['mean_err'], tested_instances, 'Błąd względny [%]', 'err')
    plot_heatmap(xlabels, ylabels, data_points['vl_vg']['mean_time'], tested_instances, 'Czas wykonania [ms]', 'time')


def plot_instances(x, y_mean, y_stddev, tested_instances, title, xlabel, file_end):
    fig, axs = plt.subplots(1, 2, figsize=(10, 5))
    for plot_type_i, plot_type in enumerate(['Średnia', 'Odchylenie standardowe']):
        y = y_mean if plot_type == 'Średnia' else y_stddev
        for iid in tested_instances:
            xy = pd.DataFrame({'x': x, 'y': y[iid]})
            xy.sort_values('x', inplace=True)
            axs[plot_type_i].plot(xy['x'], xy['y'], label=f'Instancja {iid}')
            axs[plot_type_i].set_xlabel(xlabel)
            axs[plot_type_i].set_ylabel(plot_type)
            axs[plot_type_i].legend()
            axs[plot_type_i].grid(True)
    fig.suptitle(title)
    plt.tight_layout()
    plt.savefig(f'../figures/param_inst_{file_end}')
    plt.close()


def plot_instances_mean(ax, x, y, tested_instances, xlabel, ylabel):
    instance_values = np.zeros((len(tested_instances), len(x)))
    for i, iid in enumerate(tested_instances):
        instance_values[i] = np.array(y[iid])
    mean_values = np.mean(instance_values, axis=0)
    std_values = np.std(instance_values, axis=0)

    coords = pd.DataFrame({'x': x, 'y': mean_values, 'err': std_values})
    coords.sort_values('x', inplace=True)
    ax.errorbar(coords['x'], coords['y'], yerr=coords['err'])
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_ylim(bottom=0)
    ax.grid(True)


def plot_heatmap(xlabels, ylabels, raw_values, tested_instances, title, file_end):
    fig, axs = plt.subplots(1, 2, figsize=(17, 8))

    instance_values = np.zeros((len(tested_instances), len(xlabels) * len(ylabels)))
    for i, iid in enumerate(tested_instances):
        instance_values[i] = np.array(raw_values[iid])
    mean_values = np.mean(instance_values, axis=0)
    mean_values = mean_values.reshape((len(ylabels), len(xlabels)))
    std_values = np.std(instance_values, axis=0)
    std_values = std_values.reshape((len(ylabels), len(xlabels)))

    for map_type_i, map_type in enumerate(['Średnia', 'Odchylenie standardowe']):
        values = mean_values if map_type == 'Średnia' else std_values
        axs[map_type_i].imshow(values)
        axs[map_type_i].set_xticks(np.arange(len(xlabels)), labels=xlabels)
        axs[map_type_i].set_yticks(np.arange(len(ylabels)), labels=ylabels)
        plt.setp(axs[map_type_i].get_xticklabels(), rotation=45, ha="right",
                 rotation_mode="anchor")
        for i in range(len(ylabels)):
            for j in range(len(xlabels)):
                axs[map_type_i].text(j, i, np.format_float_scientific(values[i, j], 2),
                                     ha="center", va="center", color="w")
        axs[map_type_i].set_title(map_type)

    fig.supxlabel('Parametr globalnej zmiany prędkości')
    fig.supylabel('Parametr lokalnej zmiany prędkości')
    fig.suptitle(title)

    plt.tight_layout()
    plt.savefig(f'../figures/param_vl_vg_{file_end}')
    plt.close()


if __name__ == '__main__':
    main()
