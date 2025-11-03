#!/usr/bin/env python3
"""
Visualization script for student database benchmark results.
Generates plots comparing three database implementation variants.
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def plot_benchmark_results():
    # Read benchmark data
    df = pd.read_csv('build/benchmark_results.csv')
    
    # Create figure with subplots
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Database Implementation Comparison\n(Operations ratio A:B:C = 5:5:50)', 
                 fontsize=14, fontweight='bold')
    
    # Get unique variants and dataset sizes
    variants = df['Variant'].unique()
    sizes = sorted(df['DatasetSize'].unique())
    
    # Define colors for each variant
    colors = {'Variant1_HashMap': '#2ecc71', 
              'Variant2_Mixed': '#e74c3c', 
              'Variant3_Map_BST': '#3498db'}
    
    labels = {'Variant1_HashMap': 'Variant 1: HashMap (unordered_map)', 
              'Variant2_Mixed': 'Variant 2: Mixed (vector + hash)', 
              'Variant3_Map_BST': 'Variant 3: Map/BST (std::map)'}
    
    # Plot 1: Operations per 10 seconds
    ax1 = axes[0, 0]
    for variant in variants:
        variant_data = df[df['Variant'] == variant]
        ax1.plot(variant_data['DatasetSize'], variant_data['OperationsCount'], 
                marker='o', linewidth=2, markersize=8, 
                label=labels[variant], color=colors[variant])
    ax1.set_xlabel('Dataset Size (records)', fontweight='bold')
    ax1.set_ylabel('Operations Completed in 10s', fontweight='bold')
    ax1.set_title('Performance: Operations per 10 seconds')
    ax1.set_xscale('log')
    ax1.grid(True, alpha=0.3)
    ax1.legend()
    
    # Plot 2: Memory usage
    ax2 = axes[0, 1]
    for variant in variants:
        variant_data = df[df['Variant'] == variant]
        ax2.plot(variant_data['DatasetSize'], variant_data['MemoryKB'], 
                marker='s', linewidth=2, markersize=8,
                label=labels[variant], color=colors[variant])
    ax2.set_xlabel('Dataset Size (records)', fontweight='bold')
    ax2.set_ylabel('Memory Usage (KB)', fontweight='bold')
    ax2.set_title('Memory Consumption')
    ax2.set_xscale('log')
    ax2.set_yscale('log')
    ax2.grid(True, alpha=0.3)
    ax2.legend()
    
    # Plot 3: Load time
    ax3 = axes[1, 0]
    for variant in variants:
        variant_data = df[df['Variant'] == variant]
        ax3.plot(variant_data['DatasetSize'], variant_data['LoadTime'], 
                marker='^', linewidth=2, markersize=8,
                label=labels[variant], color=colors[variant])
    ax3.set_xlabel('Dataset Size (records)', fontweight='bold')
    ax3.set_ylabel('Load Time (seconds)', fontweight='bold')
    ax3.set_title('Database Load Time')
    ax3.set_xscale('log')
    ax3.grid(True, alpha=0.3)
    ax3.legend()
    
    # Plot 4: Efficiency (Operations per KB of memory)
    ax4 = axes[1, 1]
    for variant in variants:
        variant_data = df[df['Variant'] == variant]
        efficiency = variant_data['OperationsCount'] / variant_data['MemoryKB']
        ax4.plot(variant_data['DatasetSize'], efficiency, 
                marker='D', linewidth=2, markersize=8,
                label=labels[variant], color=colors[variant])
    ax4.set_xlabel('Dataset Size (records)', fontweight='bold')
    ax4.set_ylabel('Operations per KB', fontweight='bold')
    ax4.set_title('Memory Efficiency (Ops/KB)')
    ax4.set_xscale('log')
    ax4.grid(True, alpha=0.3)
    ax4.legend()
    
    plt.tight_layout()
    plt.savefig('benchmark_comparison.png', dpi=300, bbox_inches='tight')
    print("Saved: benchmark_comparison.png")
    plt.close()
    
    # Create summary table
    print("\n=== Performance Summary ===")
    for size in sizes:
        print(f"\nDataset Size: {size} records")
        size_data = df[df['DatasetSize'] == size]
        for _, row in size_data.iterrows():
            print(f"  {labels[row['Variant']]}:")
            print(f"    Operations/10s: {row['OperationsCount']}")
            print(f"    Memory: {row['MemoryKB']} KB")
            print(f"    Load time: {row['LoadTime']:.4f} s")


def plot_sort_results():
    # Read sort benchmark data
    df_sort = pd.read_csv('build/sort_results.csv')
    
    # Replace zeros with small value for log plotting (timer precision issue)
    df_sort_plot = df_sort.copy()
    df_sort_plot.loc[df_sort_plot['StandardSort'] == 0, 'StandardSort'] = 0.00005
    df_sort_plot.loc[df_sort_plot['RadixSort'] == 0, 'RadixSort'] = 0.00005
    
    fig, ax = plt.subplots(1, 1, figsize=(10, 6))
    fig.suptitle('Sorting Algorithm Comparison (Task S4: Sort by Surname, Name)', 
                 fontsize=14, fontweight='bold')
    
    # Line plot for time comparison
    ax.plot(df_sort_plot['DatasetSize'], df_sort_plot['StandardSort'], 
            marker='o', linewidth=2, markersize=8, 
            label='Standard Sort (std::sort)', color='#9b59b6')
    ax.plot(df_sort_plot['DatasetSize'], df_sort_plot['RadixSort'], 
            marker='s', linewidth=2, markersize=8,
            label='Radix Sort (MSD)', color='#f39c12')
    
    ax.set_xlabel('Dataset Size (records)', fontweight='bold')
    ax.set_ylabel('Time (seconds)', fontweight='bold')
    ax.set_title('Sorting Time Comparison')
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    # Add note about timer precision
    ax.text(0.02, 0.98, 'Note: Values < 0.0001s capped at 0.00005s (timer precision limit)', 
            transform=ax.transAxes, fontsize=8, verticalalignment='top',
            bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.3))
    
    plt.tight_layout()
    plt.savefig('sort_comparison.png', dpi=300, bbox_inches='tight')
    print("Saved: sort_comparison.png")
    plt.close()
    
    # Print summary
    print("\n=== Sort Performance Summary ===")
    for _, row in df_sort.iterrows():
        print(f"\nDataset: {row['DatasetSize']} records")
        print(f"  Standard Sort: {row['StandardSort']:.4f} s")
        print(f"  Radix Sort: {row['RadixSort']:.4f} s")
        speedup = row['StandardSort'] / row['RadixSort']
        print(f"  Speedup: {speedup:.2f}x {'(Radix faster)' if speedup > 1 else '(Standard faster)'}")


if __name__ == '__main__':
    print("Generating benchmark visualizations...")
    plot_benchmark_results()
    plot_sort_results()
    print("\nDone! Check benchmark_comparison.png and sort_comparison.png")
